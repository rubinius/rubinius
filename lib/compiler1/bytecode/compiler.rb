require 'compiler1/sexp/simple_processor'
require 'compiler1/translation/normalize'
require 'compiler1/translation/local_scoping'
require 'compiler1/sexp/composite_processor'
require 'compiler1/translation/states'

module Compiler1

module Bytecode
  
  class MethodDescription
    def initialize(name)
      @name = name
      @assembly = ""
      @literals = []
      @primitive = nil
      @file = nil
      @required = 0
      @path = []
    end
    
    def add_literal(obj)
      idx = @literals.size
      @literals << obj
      return idx
    end
    
    attr_accessor :name, :assembly, :literals, :primitive, :file
    attr_accessor :required, :path, :state, :stack_space
    
    def prepend_asm(str)
      @assembly = str + @assembly
    end
  end

  class Compiler
    
    def self.compile_file(path, flags=nil)
      sexp = File.to_sexp(path, true)
      comp = Compiler1::Bytecode::Compiler.new
      comp.import_flags(flags) if flags
      desc = comp.compile_as_script(sexp, :__script__)
      return desc.to_cmethod
    end

    def self.compile_string(string, flags=nil, filename="(eval)", line=1)
      sexp = string.to_sexp(filename, line, true)
      comp = Compiler1::Bytecode::Compiler.new
      comp.import_flags(flags) if flags
      state = RsLocalState.new
      state.uses_eval = true
      desc = comp.compile_as_method(sexp, :__eval_script__, state)
      return desc.to_cmethod
    end
    
    def initialize
      @path = []
      @current_class = nil
      @current_method = nil
      @in_class_body = false
      @in_method_body = false
      @indexed_ivars = Hash.new { |h,k| h[k] = {} }
      
      if sys = Compiler.system_hints
        @indexed_ivars.update sys
      end
      
      @flags = {
        :fast_math => true,
        :fast_system => true,
        # :fast_send_method => true,
        :fast_field_access => true,
        :fast_coerce => true,
        :visibility => [],
      }      
    end
    
    def import_flags(flags)
      if flags.kind_of? Hash
        @flags.update flags
      else
        flags.each do |flag|
          if flag.kind_of? Symbol
            name = flag
            val = true
          else
            flag = flag.to_s.gsub("-","_")
            if m = /^no_(.*)/.match(flag)
              name = m[1].to_sym
              val = false
            else
              name = flag.to_sym
              val = true
            end
          end
          
          @flags[name] = val
          if $DEBUG
            STDERR.puts "Set flag '#{name}' to #{val.inspect}."
          end
        end
      end
    end
    
    attr_reader :call_hooks
    
    @system_hints = nil
    
    def self.load_system_hints(path)
      if path.kind_of?(Hash)
        @system_hints = path
      else
        @system_hints = parse_hints(path)
      end
    end
    
    def self.system_hints
      @system_hints
    end
    
    def add_indexed_ivar(name, idx)
      @indexed_ivars[@current_class.first][name] = idx
    end
    
    def find_ivar_index(name)
      return nil unless @current_class
      idx = @indexed_ivars[@current_class.first][name]
      # puts "Looking for #{name.inspect} on #{@current_class.first.inspect}: #{idx}"
      return idx if idx
      if sup = @current_class.last
        return @indexed_ivars[sup.to_s][name]
      end
      return nil
    end
    
    def load_hints(path)
      @indexed_ivars.update Compiler.parse_hints(path)
    end
    
    def self.parse_hints(path)
      fd = File.open(path)
      line = fd.gets
      state = :init
      out = {}
      while line
        if state == :init
          name = line.strip
          cur = {}
          idx = 0
          state = :body
        elsif line == "!\n"
          state = :init
          out[name] = cur
        else
          cur["@#{line.strip}".to_sym] = idx
          idx += 1
        end
        line = fd.gets
      end
      return out
    end
    
    attr_accessor :current_class, :in_class_body, :in_method_body
            
    def as_class_body(sup)
      sup = sup.last if sup
      cur = @current_class
      @current_class = [@path.map { |i| i.to_s }.join("::"), sup]
      cm = @in_method_body
      cc = @in_class_body
      @in_method_body = false
      @in_class_body = true
      
      flags = @flags
      
      begin
        yield
      ensure
        @flags = flags
        @current_class = cur
        @in_class_body = cc
        @in_method_body = cm
      end
    end
    
    def as_module_body
      cur = @in_class_body
      @in_class_body = false
      cm = @in_method_body
      @in_method_body = false
      flags = @flags
      begin
        yield
      ensure
        @flags = flags
        @in_class_body = cur
        @in_method_body = cm
      end
    end
    
    def as_method_body(name)
      @current_method = name
      cls = @in_class_body
      @in_class_body = false
      cur = @in_method_body
      @in_method_body = true
      flags = @flags
      begin
        yield
      ensure
        @flags = flags
        @in_class_body = cls
        @in_method_body = cur
        @current_method = nil
      end
    end
    
    attr_accessor :path, :flags
    
    def compile(sx, name, state=RsLocalState.new)
      if $DEBUG_COMPILER
        puts "==================================="
        puts "  Compiling #{name}"
        puts "==================================="

        puts "Sexp:"
        STDOUT.print sx.inspect
        STDOUT.print "\n"
      end

      nx = fully_normalize(sx, state)
      @nx = nx if $DEBUG_COMPILER
      if $DEBUG_COMPILER
        puts "==================================="
        puts "Normalized Sexp:"
        p nx
      end

      meth = MethodDescription.new(name)
      meth.path = @path.dup
      meth.state = state
      
      pro = Processor.new(self, meth, state)
      
      begin
        pro.process nx if nx[0]
      rescue RuntimeError => e
        puts "Compilation error on #{pro.method.file}:#{pro.last_line}"
        raise e
      end
      
      return pro
    end
    
    def fully_normalize(x, state=RsLocalState.new)
      x = RsLocalScoper::Phase1.process(x, state)
      comp = CompositeSexpProcessor.new
      comp << RsNormalizer.new(state, true)
      comp << RsLocalScoper.new(state)
      out = comp.process(x)
      state.collapse_lvars!
      return out
    end
    
    def compile_as_method(sx, name, state=RsLocalState.new)
      begin
        pro = compile(sx, name, state)
      rescue UnknownNodeError => e
        exc = RuntimeError.new "Unable to compile '#{name}', compiled error detected. '#{e.message}'"
        exc.set_backtrace e.backtrace
        raise exc
      end
      pro.finalize("sret")
      
      meth = pro.method
      meth.stack_space = state.stack_space
      
      return meth
    end
    
    def compile_as_script(sx, name, state=RsLocalState.new)
      pro = compile(sx, name, state)
      pro.finalize("pop\npush true\nsret")
      
      meth = pro.method
      meth.stack_space = state.stack_space
      
      if meth.stack_space > 0
        meth.prepend_asm "allocate_stack #{meth.stack_space}\n"
      end
      
      return meth
    end
    
    class Processor < SimpleSexpProcessor
      def initialize(cont, meth, state)
        super()
        self.require_expected = false
        self.strict = true
        self.auto_shift_type = true
        @compiler = cont
        @method = meth
        @output = ""
        @unique_id = 0
        @state = state
        @next = nil
        @break = nil
        @system_prefix = nil
        
        @in_block = false
        @in_ensure = false
        @in_rescue = false
        @check_return = false
        
        @call_hooks = []
        @call_hooks << MetaOperatorPlugin.new(self)
        @call_hooks << SystemMethodPlugin.new(self)
        @call_hooks << NamedSendPlugin.new(self)
        @call_hooks << FieldAccessPlugin.new(self)
        @call_hooks << TypeCoercePlugin.new(self)
      end
      
      attr_accessor :state, :in_block, :method, :last_line
      
      def capture
        s = @output
        o = ""
        @output = o
        begin
          yield
        ensure
          @output = s
        end
        
        return o
      end
      
      def flag?(name)
        @compiler.flags[name]
      end
      
      attr_reader :method
      
      def unique_id
        @state.unique_id
      end
      
      def unique_lbl(prefix = '')
        if @system_prefix
          "#{@system_prefix}#{prefix}lbl#{unique_id}"
        else
          "#{prefix}lbl#{unique_id}"
        end
      end
      
      def unique_exc
        if @system_prefix
          "#{@system_prefix}exc#{unique_id}"
        else
          "exc#{unique_id}"
        end
      end
      
      def add(str)
        @output << str.strip
        @output << "\n"
      end
      
      def finalize(last)
        @method.assembly = @output
        add last
      end

      # Compile and insert a 'helper' method into the assembly
      # Used when a method should not be exposed to the public,
      # such as the body of a metaclass
      def compile_internal_method(body, name)
        meth = @compiler.compile_as_method body, name
        ss = meth.stack_space
        meth.prepend_asm "allocate_stack #{ss}" if ss > 0
        idx = @method.add_literal meth
        add "push_literal #{idx}"
        add "swap"
        add "attach #{name}"
        add "pop"
        add "send #{name}"
      end
      
      def process_true(x)
        add "push true"
      end
      
      def process_false(x)
        add "push false"
      end
      
      def process_nil(x)
        add "push nil"
      end
      
      def process_self(x)
        add "push self"
      end
      
      def process_and(x)
        process x.shift
        lbl = unique_lbl('and_')
        add "dup"
        gif lbl
        add "pop"
        process x.shift
        set_label lbl
      end
      
      def process_or(x)
        process x.shift
        lbl = unique_lbl('or_')
        add "dup"
        git lbl
        add "pop"
        process x.shift
        set_label lbl
      end
      
      def process_not(x)
        process x.shift
        tr = unique_lbl('not_') 
        ed = unique_lbl('not_')
        git tr
        add "push true"
        goto ed
        set_label tr
        add "push false"
        set_label ed
      end

      def process_negate(x)
        recv = x.shift
        if recv.last.kind_of?(Fixnum)
          process_fixnum([-recv.last])
        else
          process recv # Pass the buck
          add "send -@" # ..and then negate it
        end
      end
      
      def process_fixnum(x)
        obj = x.shift
        
        if obj < -1
          add "push #{obj.abs}"
          add "send -@"
        else
          add "push #{obj}"
        end
      end

      def process_lit(x)
        obj = x.shift
        case obj
        when Fixnum
          process_fixnum([obj])
        when Symbol
          #add "push :#{obj}"
          idx = @method.add_literal obj
          add "push_literal #{idx}"
        when Bignum
          idx = @method.add_literal obj
          add "push_literal #{idx}"
        when Float
          idx = @method.add_literal obj
          add "push_literal #{idx}"
        when Regexp
          data = [obj.source, obj.options]
          process_regex(data)
        else
          raise "Unable to handle literal '#{obj.inspect}'"
        end
      end
      
      def process_regex(x)
        str = x.shift
        opt = x.shift
        
        loc = @method.add_literal nil
        
        add "push_literal #{loc}"
        add "dup"
        add "is_nil"
        
        lbl = unique_lbl("regex_")
        add "gif #{lbl}"
        add "pop"
        
        cnt = @method.add_literal str
        add "push #{opt}"
        add "push_literal #{cnt}"
        add "push Regexp"
        add "send new 2"
        add "set_literal #{loc}"
        
        add "#{lbl}:"
      end
      
      def process_dregx(x)
        process_dstr(x)
        #add "push 0" # TODO - Fix grammar_runtime so we get the actual options for dregex nodes
        add "push Regexp"
        add "send new 1"
      end
      
      def process_dregx_once(x)        
        loc = @method.add_literal nil
        
        add "push_literal #{loc}"
        add "dup"
        add "is_nil"
        
        lbl = unique_lbl("regex")
        add "gif #{lbl}"
        add "pop"
        
        process_dstr(x)
        add "push Regexp"
        add "send new 1"
        add "set_literal #{loc}"
        set_label lbl
      end

      def process_match2(x)
        pattern = x.shift
        target = x.shift
        process target
        process pattern
        add "send =~ 1"
      end

      # TODO match3 is an optimization node where we know
      # that the left hand side was a literal regex. Right now
      # we just send the method, but if there was a regex instruction
      # or something, we could optimize this too.
      def process_match3(x)
        pattern = x.shift
        target = x.shift
        process target
        process pattern
        add "send =~ 1"
      end

      # Only call regexp special var method if $~ is_a? MatchData
      def valid_last_match(op, nth_ref=nil)
        el = unique_lbl('match_')
        ed = unique_lbl('match_')
        add "push MatchData"
        process_gvar([:$~])
        add "send is_a? 1"
        gif el
        add "push #{nth_ref}" if nth_ref
        process_gvar([:$~])
        add op
        goto ed
        set_label el
        add "push nil"
        set_label ed        
      end

      # Regexp special var handling $&, $`, $'
      def process_back_ref(x)
        kind = x.shift
        if kind == ?'
          valid_last_match "send post_match 0"
        elsif kind == ?`
          valid_last_match "send pre_match 0"
        elsif kind == ?&
          valid_last_match "send to_s 0"          
        else
          raise "'back_ref' #{kind} not implemented yet"
        end
      end

      # Handle $1, $2, ....
      def process_nth_ref(x)
        num = x.shift
        valid_last_match "send [] 1", num
      end
      
      def set_label(name)
        add "#{name}:"
      end
      
      def git(lbl)
        add "git #{lbl}"
      end
      
      def gif(lbl)
        add "gif #{lbl}"
      end
      
      def goto(lbl)
        add "goto #{lbl}"
      end
      
      def process_if(x)
        cond = x.shift
        thn = x.shift
        els = x.shift
        process cond
        
        el = unique_lbl('if_')
        ed = unique_lbl('if_')
        
        if thn and els
          gif el
          process thn
          goto ed
          set_label el
          process els
          set_label ed
        elsif !els
          gif el
          if thn
            process thn
          else
            add "push nil"
          end
          goto ed
          set_label el
          add "push nil"
          set_label ed
        elsif !thn
          git el
          process els
          goto ed
          set_label el
          add "push nil"
          set_label ed
        end
      end
      
      # Typically surrounded by something like an :iter or :loop
      # but more generally, just introduces a scope that returns a value
      def process_block(x)
        if x.empty? or x == [[nil]]
          add "push nil"        # stack maintenance
          return []
        end
        
        fin = x.pop
        
        # puts "FIXME #{@nx}: block [[nil]]" if x == [[nil]] if $DEBUG_COMPILER
        while i = x.shift
          # puts "FIXME # {@nx}: empty i: "+i if i.empty? # FIXME: When exactly does this happen?
          next if i.empty?
          process i
          add "pop"
        end
        
        # Do the last one seperate so it's value is left on the stack.
        process fin
      end
      
      def process_scope(x)
        if x.first.empty?
          x.clear
          add "push nil"        # stack un-cleanup
          return []
        end
        out = process x.shift
        x.clear
        out
      end
      
      def save_condmod
        [@next, @break, @redo]
      end
      
      def restore_condmod(n,b,r)
        @next = n
        @break = b
        @redo = r
      end
      
      def process_while(x, cond="gif")
        cm = save_condmod()
        top = unique_lbl('while_')
        bot = unique_lbl('while_')
        @break = unique_lbl('break_')
        
        check = x.shift
        body = x.shift
        check_first = x.shift
        
        if check_first
          @redo = unique_lbl('redo_')
          
          @next = top

          set_label top
          
          process check
          add "#{cond} #{bot}"
          set_label @redo
          process body
          add "pop"
          goto top
          set_label bot
          add "push nil"
          set_label @break
        else
          @next = unique_lbl("next_")
          @redo = top
          set_label top
          # set_label @redo
          process body
          add "pop"
          set_label @next
          process check
          add "#{cond} #{bot}"
          goto top
          set_label bot
          add "push nil"
          set_label @break
        end
        
        restore_condmod(*cm)
      end
      
      def process_undef(x)
        arg = x.shift
        add "push :#{arg}"
        add "push self"
        unless @in_class_body
          "send metaclass 0"
        end
        add "send undef_method 1"
      end

      def process_until(x)
        process_while(x, "git")
      end
      
      def process_loop(x)
        cm = save_condmod()
        @break = unique_lbl('break_')
        top = unique_lbl('loop_')
        @redo = top
        set_label top
        process(x.shift)
        add "pop"
        goto top
        set_label @break
        restore_condmod(*cm)
      end
      
      def process_break(x)
        # FIXME: stack cleanup?
        
        if val = x.shift
          process val
        else
          add "push nil"
        end
        
        if @break == :block
          add "soft_return"
        elsif @break
          goto @break
        else
          add "push Compile"
          add "send __unexpected_break__ 0"
        end
      end
      
      def jump_error(msg)
        cnt = @method.add_literal msg
        add "push_literal #{cnt}"
        add "push LocalJumpError"
        add "push self"
        add "set_call_flags 1"
        add "send raise 2"
      end
            
      def process_redo(x)
        # FIXME: stack cleanup?
        
        if val = x.shift
          process val
        else
          add "push nil"
        end
        
        if @redo
          goto @redo
        else
          # Compat. I'd rather this be a compile error, but for now
          # just emit code that raises an exception
          jump_error "redo used in invalid context"
        end
      end
      
      # A when clause has one of the following forms:
      # 1) when foo
      # 2) when foo, *something
      # 3) when *something
      # 4) when a,b,c,d
      def generate_when(w, nxt, post)
        w[1].shift # Remove the :array
        lst = w[1].pop 
        # lst now contains: (referring to examples above)
        # foo in example 1 
        # *something in example 2
        # *something in example 3
        # d in example 4

        if lst[0] == :when # splat!
          matcher = "__matches_when__"
          lst = lst[1] # discard :when
        else
          matcher = "==="
        end
        body = nil

        unless w[1].empty?
          body = unique_lbl('when_')
          w[1].each do |cond|
            add "dup"
            process cond
            add "send === 1"
            git body
          end
        end

        add "dup"
        process lst
        add "send #{matcher} 1"
        gif nxt
        set_label(body) if body
        process w[2]
        add "goto #{post}" if post
      end
      
      def process_case(x)
        recv = x.shift
        whns = x.shift
        els = x.shift
        
        lbls = []
        (whns.size - 1).times { lbls << unique_lbl('case_') }
        # Even if there is no 'else', we still need to manage the stack later
        els_lbl = unique_lbl('case_')
        lbls << els_lbl

        post = unique_lbl('case_')
        lbls << post
        
        single = (whns.size == 1 and !els)
        
        # nxt = post
        
        process recv
                
        cur = nil
        w = whns.shift
        generate_when w, lbls[0], post
        
        lst = whns.pop
        
        whns.each do |w|
          cur = lbls.shift
          set_label cur
          generate_when w, lbls[0], post
        end
        
        if lst
          set_label(lbls.shift)
          generate_when lst, els_lbl, post
        end
                
        if els
          set_label(lbls.shift)
          process(els)
        else # empty or missing 'else' clause is treated as a nil
          set_label(lbls.shift)
          add "push nil"
        end
        
        set_label(post)
        add "swap"
        add "pop"
      end
      
      def process_lasgn(x)
        name = x.shift
        lv = x.shift
        idx = x.shift
        
        if val = x.shift
          process val
        end
        
        add lv.set_assembly(idx)
      end

      def process_svalue(x)
        process(x.shift)
        add "cast_array"
        add "dup"
        add "push 1"
        add "swap"
        add "send size 0"
        add "send <= 1"
        lbl = unique_lbl("svalue")
        gif lbl
        add "push 0"
        add "swap"
        add "send at 1"
        set_label lbl
      end

      def process_splat(x)
        process(x.shift)
        add "cast_array"
      end

      def process_op_asgn_or(x)
        process x.shift #lvar
        lbl = unique_lbl('asgn_or_')
        add "dup"
        git lbl
        add "pop"
        process x.shift #rhs
        set_label lbl
      end

      def process_op_asgn_and(x)
        # x &&= 7
        # yields:
        # [:op_asgn_and, [:lvar, :x, 2], [:lasgn, :x, 2, [:lit, 7]]]]
        process x.shift #lvar
        lbl = unique_lbl('asgn_and_')
        add "dup"
        gif lbl
        add "pop"
        process x.shift #rhs
        set_label lbl
      end
      
      def process_op_asgn2(x)
        # sample sexp from executing:
        # x.val ||= 6
        # [:op_asgn2, [:lvar, :x, 2], :val, :or, :val=, [:lit, 6]]]
        # x.val &&= 7
        # [:op_asgn2, [:lvar, :x, 2], :val, :and, :val=, [:lit, 7]]]
        # x.val = 6
        # [:attrasgn, [:lvar, :x, 2], :val=, [:array, [:lit, 6]]],
        #
        lvar = x.shift #lvar
        lbl = unique_lbl('asgn2_')
        msg = x.shift
        operator = x.shift # :and or :or
        msg2 = x.shift #assignment
        msg2 = (msg2.to_s << "=").intern if msg2.to_s[-1,1] != "=" # patch a grammar.y issue here for now
        val = x.shift
        
        process(lvar)
        add "dup"
        add "send #{msg}"
        if [:or, :and].include? operator
          add "dup"
          git(lbl) if operator == :or
          gif(lbl) if operator == :and
          add "pop"
          process val
        else
          process val
          add "swap"
          add "send #{operator} 1"
        end
        add "swap"

        add "send #{msg2} 1"
        nd = unique_lbl('asgn2_')
        goto nd

        set_label lbl
        add "swap"
        add "pop"

        set_label nd
      end
      
      def process_op_asgn1(x)
        # sample sexp from executing:
        # Thread.current[:__current_actor__] ||= Actor.new(current_mailbox)
        # [:op_asgn1, 
        #     [:call, [:const, :Thread], :current, [:array]], 
        #     :or, 
        #     [:array, [:call, [:const, :Actor], :new, 
        #         [:array, [:vcall, :current_mailbox]]], 
        #         [:lit, :__current_actor__], [:nil]
        #     ]
        # ]
        #
        obj = x.shift
        lbl = unique_lbl('asgn1_')
        operator = x.shift # :and, :or, :+, :- etc
        arg = x.shift
        arg.shift
        val = arg.shift
        idx = [arg.shift]
        while (huh = arg.shift) != [:nil] do
          idx << huh
        end
        idx_count = idx.size
        
        process(obj)
        add "dup"
        idx_code = idx.inject("") do |code,i|
          capture do
            process i
            add "swap"
          end << code
        end
        @output << idx_code
        add "send [] #{idx_count}"

        if [:or, :and].include? operator
          add "dup"
          git(lbl) if operator == :or
          gif(lbl) if operator == :and
          add "pop"
          process val
        else
          process val
          add "swap"
          add "send #{operator} 1"
        end
        add "swap"
        
        @output << idx_code
        add "send []= #{idx_count+1}"
        nd = unique_lbl('asgn1_')
        goto nd

        set_label lbl
        add "swap"
        add "pop"
        
        set_label nd
      end
      
      def process_lvar(x)
        name = x.shift
        lv = x.shift
        idx = x.shift
        
        unless lv
          raise "Unprocessed lvar '#{name}' detected!"
        end
        
        add lv.access_assembly(idx)
      end
      
      def process_array(x)
        sz = x.size
        while i = x.shift
          process i
        end
        
        add "make_array #{sz}"
      end
      
      def process_zarray(x)
        add "make_array 0"
      end
      
      def process_hash(x)
        sz = x.size
        i = 0
        until x.empty?
          v = x.pop
          k = x.pop
          
          process v
          process k
          
          i += 1
        end
        
        add "push Hash"
        add "send [] #{sz}"
      end

      # Implicit hashes from method calls
      alias process_ihash process_hash
      
      # TODO - x = 1,2,*[5,6]
      def process_argscat(x)
        ary = x.shift        
        itm = x.shift
        process itm
        ary.shift
        
        add "cast_array_for_args #{ary.size}"
        add "push_array"
        ary.reverse.each do |i|
          process i
        end
      end
      
      def process_ivar(x)
        name = x.shift
        if idx = @compiler.find_ivar_index(name)
          add "push_my_field #{idx}"
        else
          add "push #{name}"
        end
      end
      
      # TODO - Does this handle Regexp TLVs properly?: $1, $2, etc.
      def process_gvar(x)
        kind = x.shift
        if kind == :$!
          add "push_exception"
        else
          idx = @method.add_literal kind
          add "push_literal #{idx}"
          add "push_cpath_top"
          add "find Globals"
          add "send [] 1"
        end
      end

      def process_valias(x)
        nw = x.shift
        curr = x.shift

        add "push :#{curr}"
        add "push :#{nw}"
        add "push_cpath_top"
        add "find Globals"
        add "send add_alias 2"
      end

      # TODO Need to ensure that $~ only cotains nil or MatchData
      def process_gasgn(x)
        kind = x.shift
        idx = @method.add_literal kind
        process x.shift
        add "push_literal #{idx}"
        add "push_cpath_top"
        add "find Globals"
        add "send []= 2"
      end
      
      def process_iasgn(x)
        name = x.shift
        process x.shift
        if idx = @compiler.find_ivar_index(name)
          add "store_my_field #{idx}"
        else
          add "set #{name}"
        end
      end
      
      def process_const(x)
        add "push #{x.shift}"
      end
      
      def process_colon2(x)
        process x.shift
        add "find #{x.shift}"
      end
      
      def process_colon3(x)
        add "push_cpath_top"
        add "find #{x.shift}"
      end
      
      def process_cdecl(x)
        simp = x.shift
        val = x.shift
        complex = x.shift
        
        if simp
          process val
          add "set #{simp}"
        else
          # This should always be a colon2
          name = complex.last
          process complex[1]
          process val
          add "set +#{name}"
        end
      end
      
      def process_to_ary(x)
        process x.shift
        add 'cast_array'
      end
      
      def process_sclass(x)
        receiver = x.shift
        process receiver
        body = x.shift
        add "dup"
        add "send __verify_metaclass__ 0" # Raises TypeError if unsupported
        add "pop"
        add "open_metaclass"
        add "dup"
        compile_internal_method body, :__metaclass_init__
      end

      def process_class(x)
        name = x.shift
        sup = x.shift
        body = x.shift

        @compiler.flags[:visibility] << :public
        
        # add "push_encloser"
        
        under = false
        if name.size != 2
          under = name[1]
          process under
        elsif name.first == :colon3
          # We're opening a top-level class
          under = true
          add "push Object"
        end
        
        name = name.last
        
        if sup.nil?
          supr = nil
          add "push nil"
        else
          supr = sup.dup
          process sup
        end
        
        if under
          add "open_class_under #{name}"
        else
          add "open_class #{name}"
        end
        add "dup"
        
        # Log.debug "Compiling class '#{name}'"
        
        @compiler.path << name
        meth = nil
        @compiler.as_class_body(supr) do
          meth = @compiler.compile_as_method body, :__class_init__
        end
        @compiler.path.pop
        
        idx = @method.add_literal meth
        ss = meth.stack_space
        if ss > 0
          meth.prepend_asm "allocate_stack #{ss}\npush self\nset_encloser\n"
        else
          meth.prepend_asm "push self\nset_encloser\n"
        end
        add "push_literal #{idx}"
        add "swap"
        add "attach __class_init__"
        add "pop"
        add "send __class_init__"
        add "push_encloser"
        # add "set_encloser"

        @compiler.flags[:visibility].pop
      end
      
      def process_module(x)
        name = x.shift
        body = x.shift

        @compiler.flags[:visibility] << :public
        
        # add "push_encloser"
        
        under = false
        if name.size != 2
          under = name[1]
          process under
        elsif name.first == :colon3
          # We're opening a top-level module
          under = true
          add "push Object"
        end

        name = name.last
        
        if under
          add "open_module_under #{name}"
        else
          add "open_module #{name}"
        end
        add "dup"
        
        @compiler.path << name
        meth = nil
        @compiler.as_module_body do
          meth = @compiler.compile_as_method body, :__module_init__
        end
        @compiler.path.pop
        
        idx = @method.add_literal meth
        ss = meth.stack_space
        if ss > 0
          meth.prepend_asm "allocate_stack #{ss}\npush self\nset_encloser\n"
        else
          meth.prepend_asm "push self\nset_encloser\n"
        end
        
        add "push_literal #{idx}"
        add "swap"
        add "attach __module_init__"
        add "pop"
        add "send __module_init__"
        add "pop"
        add "push self"
        add "push_encloser"
        # add "set_encloser"

        @compiler.flags[:visibility].pop
      end
      
      def process_begin(x)
        b = @break
        @break = unique_lbl('begin_')
        process x.shift
        set_label @break
        @break = b
      end

      def process_rescue(x)
        ex = unique_exc()
        fin = unique_lbl('rescue_')
        rr = unique_lbl('rescue_')
        old_retry = @retry_label
        @retry_label = unique_lbl('rescue_')
        set_label @retry_label
        body = x.shift
        res = x.shift
        els = x.shift

        if body && body.first == :resbody
          if !res # no 'else' clause
            # No body, nothing to execute, so just skip it.
            add "push nil"
            return
          else
            # Empty begin clause, but there is an 'else' to execute
            process res # actually the 'else'
            return
          end
        end
        
        add "#exc_start #{ex}"
        process body
        # If we reach here, then no exception has been thrown, so
        # we skip all the code that checks exceptions, etc.
        if els
          add "pop"
          process els
        end
        goto fin
        add "#exceptions #{ex}"
        ir = @in_rescue
        @in_rescue = true
        do_resbody res, rr, fin
        @in_rescue = ir
        set_label rr
        add "push_exception"
        add "raise_exc"
        set_label fin

        # Since this is always the end if the exception has either
        # not occurred or has been correctly handled, we clear the current
        # exception. There is an optimization that could be done here,
        # by setting up the jumps so that we only execute the clear
        # when we actually ran any exception handling code. But it doesn't
        # hurt to always do it for now.
        add "clear_exception"
        add "#exc_end #{ex}"
        @retry_label = old_retry 
      end

      def process_retry(x)
        unless @retry_label
          jump_error "retry used in invalid context"
          return
        end
        
        if @retry_label == :block
          jump_error "retry in blocks not supported"
        else
          goto @retry_label
        end
      end

      # There are two rough groups of arguments that can be passed to :defined?:
      # 1. Arguments that are always defined, and always return a known string.
      # e.g. "x = 5" is always 'assignment', whether 'self#x=' is a method or not.
      # 2. Arguments that need code executed at runtime to determine their status.
      # e.g. "SomeModule::SomeClass::foo". 'foo' could have been defined as a class
      # method at any time, so there is no compile-time way to make that determination.
      # [:defined, [:lasgn, :x, 2, [:lit, 5]]]
      # All that being said, the behavior of MRI here is insane, and it's not getting
      # implemented unless somebody pays me to do it.
      def process_defined(x)
        expr = x.shift
        # if something is defined, !something is too.
        # if !something is undefined, then so is something.
        expr.shift if expr[0] == :not
        return(reject_defined) if expr.flatten.include?(:newline) # grouped expression == evil

        node = expr.shift

        static_nodes = [:self, :nil, :true, :false, :lit, :lasgn, :gasgn, :iasgn, :cdecl, :cvdecl, :cvasgn, :lvar, :str, :array, :hash]
        if static_nodes.include?(node)
          add "push true"
          return
        end

        # TODO - Move this to the normalizer
        if node == :vcall # normalize vcalls into calls, a la process_vcall
          node = :call
          expr = [[:self], expr.shift, [:array]]
        end

        case node
          when :call
            node_type = expr[0].first
            
            # Make sure there are no args.
            if expr.last.size > 1
              return(reject_defined) 
            end
            
            receiver = expr.shift
            msg = expr.shift # method name
            
            if receiver[0] == :const              
              lbl = unique_lbl('defined_')
              const = receiver[1]
              add "push :#{const}"
              add "push Object"
              add "send const_defined? 1"
              add "dup" # leave the boolean on the stack
              gif lbl # if the const doesn't exist, it clearly can't respond
              add "push :#{msg}"
              add "push #{const}"
              add "send respond_to? 1"
              set_label(lbl)
            else
              add "push :#{msg}"
              process receiver
              add "send respond_to? 1"
            end
          when :cvar
            cvar = expr.shift
            add "push :#{cvar}"
            add "push true" # class vars as symbols, not strings
            add "push self"
            add "send class_variables 1"
            add "send include? 1"
          when :gvar
            add "push :#{expr.shift}"
            add "push Globals"
            add "send key? 1"
          when :ivar
            ivar = expr.shift
            add "push :#{ivar}"
            add "push true" # instance vars as symbols, not strings
            add "push self"
            add "send instance_variables 1"
            add "send include? 1"
          when :yield
            add "push true" # conform to "all primitives have a self" rule
            add "send_primitive block_given 0"
          when :const
            add "push :#{expr.shift}"
            add "push Object"
            add "send const_defined? 1"
          when :colon2, :colon3
            str = ""
            until expr.empty?
              # Convert the constant parse tree into a string like ::Object::SomeClass
              str = const_to_string(expr, str)
            end
            lit = @method.add_literal str
            add "push_literal #{lit}"
            add "push Object"
            add "send const_defined? 1"
          else
            reject_defined
          end
      end

      def reject_defined
        if $DEBUG
          STDERR.puts "Passed a complex expression to 'defined?'. This is why we can't have nice things."
        end
        add "push false"
      end
  
      # e.g. [[:const, :Object], :Blah]
      # e.g. [[:colon3, :Foo], :Bar]
      # e.g. [[:colon2, [:colon3, :Faz], :Boo], :Batch]
      # e.g. [[:colon2, [:const, :Zizz], :Koom], :Yonk]
      # TODO - There is probably a better way, but it is late. Really late.
      def const_to_string(tree, str)
        return str if tree.empty?
        piece = tree.shift
        unless str[-2,2] == "::" || str == ""
          str << "::"
        end
        if piece.is_a?(Array)
          str = const_to_string(piece, str) until piece.empty?
          str
        elsif [:const, :colon2].include?(piece)
          str
        elsif piece == :colon3
          "::" + str
        else
          str << "#{piece}"
        end
      end

      def do_resbody(x, rr, fin)
        x.shift
        cond = x.shift
        body = x.shift
        other = x.shift
        
        if cond.nil?
          cond = [[:const, :StandardError]]
        else
          cond.shift
        end
        
        if other.nil?
          nxt = rr
        else
          nxt = unique_lbl('resbody_')
        end
        
        one_cond = (cond.size == 1)
        
        bl = unique_lbl('resbody_')
        
        # If Object is your condition, we run it without checking
        # since everything is an object.
        unless cond == [[:const, :Object]]
        
          while cur = cond.shift
            add "push_exception"
            process cur
            add "send === 1"
            if cond.empty?
              gif nxt
            else
              git bl
            end
          end
          
        end
        
        if !one_cond
          add "#{bl}:"
        end
        
        if body
          process body
        else
          add "push nil"
        end
        
        goto fin
        
        if !other.nil?
          add "#{nxt}:"
          do_resbody other, rr, fin
        end
      end
      
      def process_ensure(x)
        body = x.shift
        ens =  x.shift
        
        unless body
          # OMGWTFBBQ a totally empty ensure. stupid.

          unless ens == [:nil]
            process ens
            # Throw away the result of the ensure block
            add "pop"
          end
          
          # The pathetic return value of this utterly worthless ensure.
          add "push nil"

          return
        end
        
        # For some reason, if the ensure is empty, it shows up like this
        # instead of just a nil *shrug*.
        if ens == [:nil]
          process body
          return
        end

        # Per MRI, we use body's result value/exception unless ens raises.
        
        ex = unique_exc()
        add "#exc_start #{ex}"
        
        ie = @in_ensure
        @in_ensure = true
        cr = @check_return
        process body
        should_check = @check_return
        # If there is an outer ensure, don't reset check_return
        # let it bubble out.
        unless ie
          @check_return = cr
        end
        @in_ensure = ie
        
        sp = @system_prefix
        
        replace = "#{unique_id}XX-ES"
        @system_prefix = replace
        
        ensure_code = capture do
          process ens
        end
        
        @system_prefix = sp
        
        ran_ok =  unique_lbl("ensure_")
        
        goto ran_ok
                
        add "#exceptions #{ex}"
        add "push_exception"
        @output << ensure_code
        add "pop"
        # @in_ensure is set here if there is an outer ensure
        # also. if there is, we skip the return logic here
        # and just raise the return value exception.
        if should_check and !@in_ensure
          add "push_exception"
          add "dup"
          add "push Tuple"
          add "swap"
          add "kind_of"
          not_tuple = unique_lbl("ensure_")
          gif not_tuple
          add "push 0"
          add "fetch_field"
          if @in_block
            add "ret"
          else
            add "sret"
          end
          set_label not_tuple
        end
        
        add "raise_exc"
        
        add "#exc_end #{ex}"
        set_label ran_ok
        
        # we can't have duplicate labels, so we recode the 
        # all labels with a new prefix, replacing the system
        # prefix we applied to the original code.
        @output << ensure_code.gsub(replace, "#{unique_id}XX-ES")
        
        # pop the ensure_code's value, leaving the body's value
        add "pop"
      end
      
      def process_return(x)
        val = x.shift
        
        if @in_rescue
          add "clear_exception"
        end
        
        if val
          if val.first == :argscat
            ary =  val[1]
            item = val[2]
            process item
            process ary
            add "send << 1"
          else
            process val
          end
        else
          add "push nil"
        end
       
        if @in_ensure
          add "make_array 1"
          add "cast_tuple"
          add "raise_exc"
          @check_return = true
        elsif @in_block
          add "ret"
        else
          add "sret"
        end
      end
      
      # Handles various types of multiple assignment, including do |*args| .. end
      # argscat and argspush are handled elsewhere
      def process_masgn(x, block_args=false)
        return unless x[0] # masgn to empty name, e.g. do |*| .. end
        if x[0][0] == :array
          lhs = x.shift
        else # splatted block arguments have no lhs
          lhs = []
        end
        splat = x.shift
        source = x.shift

        if source
          # The sexp has 2 nodes that do the same thing. It's annoying.
          if source[0] == :to_ary or source[0] == :splat
            source.shift # get rid of to_ary or splat
            process source.shift
            add "cast_tuple"
            add "dup"
          elsif source[0] == :array # e.g. x,y = 5,6
            handle_array_masgn lhs, splat, source
            return
          else
            puts "Please open a ticket with the source code that printed this! " \
            "masgn 'source' was not to_ary, splat, or array: #{source.inspect}"
            process source
          end
        else
          if @in_block == :lambda
            # Stupid other behavior for lambda/proc
            add "cast_tuple"
          elsif block_args
            add "passed_blockarg 1"
            lbl = unique_lbl("bamasgn_")
            gif lbl
            add "dup"
            add "push 0"
            add "fetch_field"
            add "dup"
            add "push Array"
            add "swap"
            add "kind_of"
          
            lbl2 = unique_lbl("bamasgn_")
            gif lbl2
            # swap the original tuple with the extract array
            add "swap"
          
            # Remove the thing on the top, either the tuple or the
            # thing that wasn't an array
            set_label lbl2
            add "pop"          
            set_label lbl
            # No source implies that the source exists on the stack
            # already. Make sure it's a tuple first.
            add "cast_tuple"
          else
            add "cast_tuple"
          end
        end

        # This is bypassed for code like: def foo;yield(1,2,3);end  foo {|*args| ...}
        unless lhs.empty? # masgn to multple lhs
          lhs.shift # get rid of :array
          lhs.each do |part|
            add "unshift_tuple"
            process part
            add "pop"
          end
        end
        
        if splat
          # Code such as: x,*y = *[5,6,7]
          # executes the lhs.empty? conditional above as well as this section
          add "cast_array"
          process splat
          add "pop"
          add "cast_array"
        elsif source # We get here with code like: x,y = foo_method(1,2)
          add "pop"
          add "cast_array"
        else
          # Pull the tuple with the implicit source off the stack
          add "pop"
        end
      end

      def process_single_block_var(x)
        lhs = x.shift
        lbl =  unique_lbl("ba_")
        lbl2 = unique_lbl("ba_")
        lbl3 = unique_lbl("ba_")
        
        add "passed_blockarg 0"
        gif lbl3
        add "pop"
        add "push nil"
        goto lbl2
        
        set_label lbl3
        add "passed_blockarg 1"
        git lbl
        
        add "cast_array"
        goto lbl2
        
        set_label lbl
        # add "dup"
        # add "push self"
        # add "send p 1"
        # add "pop"
        add "push 0"
        add "fetch_field"
        
        set_label lbl2
        process lhs
        add "pop"
      end
      
      # For now, masgn is deliberately non-compliant with MRI wrt the return val from an masgn.
      # Rubinius returns true as the result of the assignment, but MRI returns an array
      # containing all the elements on the rhs. As this result is never used, the cost
      # of creating and then discarding this array is avoided
      def handle_array_masgn(lhs, splat, source)        
        lhs.shift
        source.shift
        source_size = source.size
        lhs_size = lhs.size

        if splat
          source.each do |e|
            process e
          end
          # for example: a,b,*c = 99,8
          (lhs_size - source_size).times do
            add "push nil"
          end

          # Convert un-popped entries to an array
          sz = source_size - lhs_size
          sz = 0 if sz < 0 # Empty splats become [] instead of nil
          add "make_array #{sz}" 
          process splat
          add "pop"
          lhs.reverse.each do |e|
            if e.first == :masgn
              process e
            else
              process e
              add "pop"
            end
          end
        else
          # for example: a,b,c = 99,8
          (lhs_size - source_size).times do
            add "push nil"
          end

          source.reverse.each do |e|
            process e
          end
        
          lhs.each do |e|
            if e.first == :masgn
              process e
            else
              process e
              add "pop"
            end
          end
          delta = source_size - lhs_size
          delta.times { add "pop" } # Never happens unless delta > 0
        end

        add "push true" # masgns return true, since noone uses the array of rhs values MRI returns
      end
      
      def detect_primitive(body)
        return unless body && body[1] && body[1][1]
        cl = body[1][1]
        if cl.first == :newline
          cl = cl.last
        end
        found = detect_special(:primitive, cl)
        if found
          b = body[1]
          b.shift
          b.shift
          # Detect that a primitive is used with no
          # handling for if the primitive fails.
          if b.empty?
            # puts "Using default primitive fallback for #{found}"
            msg = [:str, "Primitive #{found} failed."]
            exc = [:call, [:const, :ArgumentError], :new, [:array, msg]]
            b.unshift [:call, [:self], :raise, [:array, exc]]
          end
          b.unshift :block
          body[1] = b
          return found
        end
        
        return nil
      end

      def detect_special(kind, cl)
        #[:call, [:const, :Ruby], :asm, [:array, [:str, "push 1\nsend puts 1", 0]]]]
        if cl.kind_of?(Array) and cl[0,3] == [:call, [:const, :Ruby], kind]
          args = cl[3]
          args.shift

          if args.length == 1
            ary = args.first # should be [:str, "foo"] or [:lit, :bar]
            if ary.kind_of? Array
              if [:str, :lit].include?(ary.first)
                return ary[1]
              else ary.first == :ihash or ary.first == :hash
                hsh = {}
                ary.shift
                while e = ary.shift
                  val = ary.shift
                  if val.first == :lit
                    val = val.last
                  elsif val.first == :true
                    val = true
                  elsif val.first == :false
                    val = false
                  elsif val.first == :nil
                    val = nil
                  end
                  hsh[e.last] = val
                end
                return hsh
              end
            end
          end
        end
        
        return nil
      end
      
      def process_defs(x)
        s = @output
        str = ""
        @output = str
        process(x.shift) # receiver
        @output = s
        process_defn x, "attach_method", str
      end
      
      def process_defn(x, kind="add_method", recv="push_self")
        name = x.shift
        args = x.shift
        body = x.shift
                
        prim = detect_primitive(body)
        state = RsLocalState.new
        scoper = RsLocalScoper.new(state)

        # Required arguments, added in order so we know
        # their positions.
        if required_args = args[1]
          # HACK OptParse::NoArgument.incompatible_argument_styles empty body
          args[1] = required_args = Array(required_args)
          required_args.each { |e| state.add_arg e }
        end
        
        defaults = args[4]
        # The initializers for default argument values may contain
        # references to other arguments, and/or unprocessed lvars.
        # Also, it needs to share a scope with this method definition
        if defaults
          defaults.shift
          defaults.each_with_index do |node,i|
            defaults[i] = scoper.process(node)
            var = defaults[i]
            lv = state.add_arg(var[1])
            lv.disallow_stack_access!
            lv.has_default = true
            # state.disallow_stack_access var[1]
            # state.find_local(var[1], true, true)
          end
        end
        
        # Splat argument
        if args[3]
          state.arg_splat = args[3].first
        end
        
        ba_name = nil
        if args.last and args.last.first == :block_arg
          ba = args.last
          state.arg_block = ba[1]
        end
        
        meth = nil
        line = @last_line
        @compiler.as_method_body(name) do
          meth = @compiler.compile_as_method body, name, state
        end
        
        meth.primitive = prim if prim
        idx = @method.add_literal meth
        add "push_literal #{idx}"
        add recv
        
        # Indicate to add_method that this is a private method.
        if kind == "add_method" and @compiler.flags[:visibility].last == :private
          add "set_call_flags 1"
        end
        
        add "#{kind} #{name}"
        str = "#line #{line}\n"
                
        required = args[1] ? args[1].size : 0
        
        # Copy all the non-stack, not-default args into the locals tuple
        state.each_arg do |_, var|
          if !var.on_stack? and !var.has_default
            str << "set_local_from_fp #{var.slot} #{var.stack_position}\n"
          end
        end
        
        max = min = required
                
        if defaults
          defaults.each do |var|
            lv = state.local(var[1])
            lbl = unique_lbl('set_')
            lbl2 = unique_lbl('set_')
            #lbl = "set#{state.unique_id}"
            str << "passed_arg #{lv.argument_position}\ngit #{lbl}\n"
            save = @output
            @output = ""
            cur = @method
            @method = meth
            process var.last
            str << @output
            @output = save
            @method = cur
            str << "set #{var[1]}:#{lv.slot}\npop\ngoto #{lbl2}\n"
            str << "#{lbl}: set_local_from_fp #{lv.slot} #{lv.stack_position}\n#{lbl2}:\n"
          end
          
          max += defaults.size
        end
                
        if state.arg_splat
          splat_name, lv = state.arg_splat
          str << "make_rest_fp #{state.number_of_arguments}\n"
          if lv.on_stack?
            str << "set_local_fp #{lv.alloca}\npop\n"
          else
            str << "set #{splat_name}:#{lv.slot}\npop\n"
          end
          max = 1024
        end
        
        if state.arg_block
          block_name, lv = state.arg_block
          str <<  "push_block\npush Proc\nsend from_environment 1\n"
          if lv.on_stack?
            str << "set_local_fp #{lv.alloca}\npop\n"
          else
            str << "set #{block_name}:#{lv.slot}\npop\n"
          end
        end
        
        if (args[2].nil? || args[2].empty?) && (args[3].nil? || args[3].empty?)
          meth.required = required
        else
          meth.required = -(required + 1)
        end

        # Add an opcode to allocate space for fast locals
        ss = state.stack_space
        if ss > 0
          str = "check_argcount #{min} #{max}\nallocate_stack #{ss}\n" + str
        else
          str = "check_argcount #{min} #{max}\n" + str
        end
                
        meth.prepend_asm str
      end
      
      def detect_class_special(x)
        recv = x.shift
        meth = x.shift
        args = x.shift.dup
        
        # Get rid of :array
        args.shift
        
        if recv == [:self] and meth == :ivar_as_index
          hsh = args.shift
          hsh.shift
          until hsh.empty?
            key = hsh.shift.pop
            val = hsh.shift.pop
            if key.to_s[0] != ?@
              key = "@#{key}".to_sym
            end
            @compiler.add_indexed_ivar(key, val)
          end
          
          return true
        elsif recv = [:self] and meth == :private and args.empty?
          @compiler.flags[:visibility].pop
          @compiler.flags[:visibility] << :private
          return true
        elsif recv = [:self] and meth == :public and args.empty?
          @compiler.flags[:visibility].pop
          @compiler.flags[:visibility] << :public
          return true
        end
        return false
      end
            
      def hook_call(recv, meth, args, block)
        @call_hooks.each do |plugin|
          return true if plugin.run(recv, meth, args, block)
        end
        
        return false
      end

      def process_call(x, block=false)
        line = @last_line
        
        x.unshift :call
        asm = detect_special(:asm, x)
        if asm
          add asm
          x.clear
          return
        end
        
        if flags = detect_special(:set_compiler_flags, x)
          @compiler.flags.update flags
          x.clear
          return
        end
        
        x.shift
                
        if @compiler.in_class_body
          out = detect_class_special(x.dup)
          if out
            x.clear
            add "push nil"
            return
          end
        end
        
        recv = x.shift
        meth = x.shift
        args = x.shift
        options = x.shift
        # recv [:lvar, :f, 3]
        # meth :[]=
        # args [:argspush, [:newline, 20, "argscat.rb", [:splat, [:lvar, :x, 2]]], [:lit, 1]]
        # options nil
        
        if meth == :block_given?
          add "push true"
          add "send_primitive block_given 0"
          return
        end
        
        return if hook_call(recv, meth, args, block)
        
        grab_args = false
        
        if args
          if args.first == :argscat
            process(args)
            sz = "+"
            grab_args = true
          elsif args.first == :argspush # e.g. f[*x] = 1
            args.shift
            process(args.pop)
            sz = "+"
            args.reverse.each { |a| process(a) }
            add "cast_array_for_args #{args.size}"
            add "push_array"
            grab_args = true
          else
            args.shift
            args.reverse.each { |a| process(a) }
            sz = args.size
          end
        else
          sz = nil
        end
        
        if block
          @post_send = ps = unique_lbl('ps_')
        end
        
        if grab_args
          add "get_args"
        end
        
        if block
          op = "&send"
          ib = @in_block
          if options and options[:function] and (meth == :lambda or meth == :proc)
            @in_block = :lambda
          else
            @in_block = true
          end
          process block
          @in_block = ib
          add "#line #{line}" if @last_line != line
        else
          add "push nil" if grab_args
          
          op = "send"
        end
        
        add "swap" if grab_args
                
        process recv
        
        if grab_args
          add "swap"
          add "set_args"
        end
        
        if options and options[:function]
          add "set_call_flags 1"
        end
        
        add "#{op} #{meth} #{sz}"
        if block
          add "#{ps}:"
        end
      end
      
      def process_zsuper(x, block=false)
        lvars = [:array]
        @state.args.each { |info| lvars << [:lvar, info[0], info[1]] }
        
        if @state.arg_splat
          code = [:argscat, lvars, [:lvar] + @state.arg_splat]
        else
          code = lvars
        end
        
        process_super([code], block)
      end
      
      def process_super(x, block=false)
        args = x.shift
        if args
          if args.first == :argscat
            process(args)
            sz = "+"
            grab_args = true
          else
            args.shift
            args.reverse.each { |a| process(a) }
            sz = args.size
          end
        else
          sz = 0
        end
        
        if block
          @post_send = ps = unique_lbl('super_')
        end
        
        add "get_args" if grab_args    
        
        if block
          process block
        else
          add "push nil"
        end
        
        if grab_args
          add "swap"
          add "set_args"
        end
        
        add "super #{@method.name} #{sz}"
        add "#{ps}:" if block
      end
      
      def process_attrasgn(x)
        # The following line would be a ||=, but that doesn't work either at the moment
        x[0] = [:self] unless x[0] # grammar.y NODE_ATTRASGN provides a nil for self
        if x[1].to_s[-1,1] != "=" # patch a grammar.y issue here for now
          x[1] = (x[1].to_s << "=").intern
        end
        process_call x
      end
      
      # Encountered when calling:  foo_method(&block), or foo_method(*args, &block)
      def process_block_pass(x)
        blk = x.shift
        cl = x.shift
        cl.shift
        process_call cl, blk
      end

      # Method calls that take a block are wrapped in this node
      # Fairly heavily normalized before we get here
      def process_iter(x)
        cl = x.shift
        args = x.shift
        body = x.shift
        locals = x.shift
     
        kind = cl.shift

        # Prevent single block arguments from using the more-complex masgn code
        # TODO - Move this to the normalizer
        if args and (args[0] == :lasgn || args[0] == :iasgn)
          args = [:single_block_var, args] 
        elsif args and args[0] != :masgn
          puts "OPEN A TICKET: Unexpected iter args: #{args.inspect}"
        end

        iter = [:block_iter, args, body, locals]
        if kind == :call
          process_call cl, iter
        elsif kind == :super
          process_super cl, iter
        else
          process_zsuper cl, iter
        end
      end
      
      def process_block_iter(x)
        args = x.shift
        body = x.shift
        locals = x.shift

        one = unique_lbl('iter_')
        two = unique_lbl('iter_')

        if locals
          local_idx = @method.add_literal locals
          add "push_literal #{local_idx}"
        else
          add "push nil"
        end
        add "push &#{@post_send}"
        add "push &#{one}"
        add "create_block 255"

        brk = @break
        @break = :block
        goto two

        if args
          if args.first == :masgn
            args.shift
            process_masgn args, true
          else
            process args
          end
        else
          # No args, but we still have to remove the block args tuple from
          # the stack.
          add "pop"
        end
        
        red = @redo
        @redo = unique_lbl('redo_')
        set_label @redo
        ret = @retry_label
        @retry = :block
        process body
        add "#{one}: soft_return"
        set_label two
        @redo = red
        @break = brk
        @retry_label = ret
      end
      
      def process_xstr(x)
        str = x.shift
        cnt = @method.add_literal str
        add "push_literal #{cnt}"
        add "push self"
        add "set_call_flags 1"
        add "send ` 1"
      end
      
      def process_str(x)
        str = x.shift
        cnt = @method.add_literal str
        add "push_literal #{cnt}"
        # The string dup is strings work the same as the do in CRuby, 
        # ie that every declaration of them is a new one.
        add "string_dup"
      end
      
      def process_static_str(x)
        str = x.shift
        cnt = @method.add_literal str
        add "push_literal #{cnt}"
        # We don't dup it, meaning that every call will get the same one.
        # This gives ruby a decent static string buffer object.
      end
      
      def process_evstr(x)
        process x.shift
        add "send to_s"
      end
      
      def process_dxstr(x)
        str = x.shift
        cnt = 0
        while y = x.pop
          process y
          cnt += 1
        end
        lit = @method.add_literal str
        add "push_literal #{lit}"
        add "string_dup"
        cnt.times { add "string_append" }
        add "push self"
        add "set_call_flags 1"
        add "send ` 1"
      end
      
      def process_dstr(x)
        str = x.shift
        cnt = 0
        while y = x.pop
          process y
          cnt += 1
        end
        lit = @method.add_literal str
        add "push_literal #{lit}"
        add "string_dup"
        cnt.times { add "string_append" }
      end
      
      def process_dsym(x) # Symbols with String interpolation
        # Unpack, process contained String and have it symbolise itself
        process x.shift
        add 'send to_sym 0'
      end

      def process_yield(x)
        args = x.shift
        # lit indicates that if the args are an array, the are to be
        # passed not as an array, but directly on the stack as multiple
        # args.
        lit = x.shift
        
        if args
          kind = args.first
          if kind == :array and lit
            args.shift
            args.reverse.each { |a| process(a) }
            sz = args.size
          elsif kind == :splat
            args.shift
            args.unshift [:array] # TODO - Fix unshift so that this can be a single call
            args.unshift :argscat
            process(args)
            add "get_args"
            sz = "+"
          else
            process(args)
            sz = 1
          end
        else
          sz = 0
        end
        
        yld = unique_lbl('yield_')
        add "push_block"
        git yld
        jump_error "no block given"
        set_label yld
        add "push_block"
        add "send call #{sz}"
      end
            
      def process_next(x)
        val = x.shift
        if val
          process(val)
        else
          add "push nil"
        end
        
        if @next
          goto @next
        elsif @in_block
          add "soft_return"
        else
          jump_error "next used in invalid context"
        end
      end
      
      def process_newline(x)
        line = x.shift
        @method.file = x.shift
        if @last_line.nil? or line != @last_line
          add "\#line #{line}"
        end
        @last_line = line
        process(x.shift)
      end

      def process_alias(x)
        cur = x.shift
        nw = x.shift
        add "push :#{cur}"
        add "push :#{nw}"
        add "push self"
        add "send alias_method 2"
      end
      
      def process_dot2(x)
        start = x.shift
        fin = x.shift
        process fin
        process start
        add "push Range"
        add "send new 2"
      end
      
      def process_dot3(x)
        start = x.shift
        fin = x.shift
        add "push true"
        process fin
        process start
        add "push Range"
        add "send new 3"
      end
      
      # Class variable support.
      def process_cvdecl(x)
        name = x.shift
        val = x.shift
        process val
        add "push :#{name}"
        add "push self"
        add "send class_variable_set 2"
      end
      
      def process_cvasgn(x)
        name = x.shift
        val = x.shift
        process val
        add "push :#{name}"
        add "push self"
        add "send class_variable_set 2"
      end
      
      def process_cvar(x)
        name = x.shift
        add "push :#{name}"
        add "push self"
        add "send class_variable_get 1"
      end
      
    end
  end
end
end

require 'compiler1/bytecode/plugins'
