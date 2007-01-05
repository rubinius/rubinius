begin
  require 'sexp/simple_processor'
  require 'translation/normalize'
  require 'translation/local_scoping'
  require 'sexp/composite_processor'
  require 'translation/states'
rescue LoadError
  STDERR.puts "Unable to load one or more required libraries. Make sure you have the 'sydparse', 'emp', and 'RubyInline' gems."
  raise $!
end

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
    attr_accessor :required, :path
  end

  class Compiler
    def initialize
      @path = []
      @current_class = nil
      @current_method = nil
      @in_class_body = false
      @in_method_body = false
      @indexed_ivars = Hash.new { |h,k| h[k] = {} }
    end
    
    def add_indexed_ivar(name, idx)
      @indexed_ivars[@current_class.first][name] = idx
    end
    
    def find_ivar_index(name)
      return nil unless @current_class
      idx = @indexed_ivars[@current_class.first][name]
      return idx if idx
      if sup = @current_class.last
        return @indexed_ivars[sup.to_s][name]
      end
      return nil
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
      
      begin
        yield
      ensure
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
      begin
        yield
      ensure
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
      begin
        yield
      ensure
        @in_class_body = cls
        @in_method_body = cur
        @current_method = nil
      end
    end
    
    attr_accessor :path
    
    def compile(sx, name, state=RsLocalState.new)
      nx = fully_normalize(sx, state)
      #require 'pp'
      #pp nx
      meth = MethodDescription.new(name)
      meth.path = @path.dup
      
      pro = Processor.new(self, meth, state)
      pro.process nx
      return pro
    end
    
    def fully_normalize(x, state=RsLocalState.new)
      comp = CompositeSexpProcessor.new
      comp << RsNormalizer.new(state, true)
      comp << RsLocalScoper.new(state)
      
      return comp.process(x)      
    end
    
    def compile_as_method(sx, name, state=RsLocalState.new)
      begin
        pro = compile(sx, name, state)
      rescue UnknownNodeError => e
        exc = RuntimeError.new "Unable to compile '#{name}', compiled error detected. '#{e.message}'"
        exc.set_backtrace e.backtrace
        raise exc
      end
      pro.finalize("ret")
      return pro.method
    end
    
    def compile_as_script(sx, name)
      pro = compile(sx, name)
      pro.finalize("push true\nret")
      return pro.method
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
      end
      
      attr_reader :method
      
      def unique_id
        @state.unique_id
      end
      
      def unique_lbl
        "lbl#{unique_id}"
      end
      
      def unique_exc
        "exc#{unique_id}"
      end
      
      def add(str)
        @output << str.strip
        @output << "\n"
      end
      
      def finalize(last)
        @method.assembly = @output
        add last
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
        lbl = unique_lbl()
        add "dup"
        add "gif #{lbl}"
        add "pop"
        process x.shift
        add "#{lbl}:"
      end
      
      def process_or(x)
        process x.shift
        lbl = unique_lbl()
        add "dup"
        add "git #{lbl}"
        add "pop"
        process x.shift
        add "#{lbl}:"
      end
      
      def process_not(x)
        process x.shift
        tr = unique_lbl() 
        ed = unique_lbl()
        git tr
        add "push true"
        goto ed
        set_label tr
        add "push false"
        set_label ed
      end
      
      def process_lit(x)
        obj = x.shift
        case obj
        when Fixnum
          add "push #{obj}"
        when Symbol
          add "push :#{obj}"
        when Bignum
          idx = @method.add_literal obj
          add "push_literal #{idx}"
        when Float
          idx = @method.add_literal obj
          add "push_literal #{idx}"
        when Regexp
          data = [obj.source]
          if obj.options & 4 == 4
            data << true
          end
          process_regex(data)
        else
          raise "Unable to handle literal '#{obj.inspect}'"
        end
      end
      
      def process_regex(x)
        str = x.shift
        opt = x.shift
        if opt
          add "push true"
        else
          add "push false"
        end
        cnt = @method.add_literal str
        add "push_literal #{cnt}"
        add "push Regexp"
        add "send new 2"
      end

      # TODO match3 is an optimization node where we know
      # that the left hand side was a literal regex. Right now
      # we just send the method, but if there was a regex instruction
      # or something, we could optimize this too.
      def process_match3(x)
        pattern = x.shift
        target = x.shift
        process pattern
        process target
        add "send =~ 1"
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
        
        el = unique_lbl()
        ed = unique_lbl()
        
        if thn and els
          gif el
          process thn
          goto ed
          set_label el
          process els
          set_label ed
        elsif !els
          gif el
          process thn
          goto ed
          set_label el
          add "push false"
          set_label ed
        elsif !thn
          git el
          process els
          goto ed
          set_label el
          add "push false"
          set_label ed
        end
      end
      
      def process_block(x)
        return [] if x == [[nil]]
        while i = x.shift
          next if i.empty?
          process i
        end
      end
      
      def process_scope(x)
        if x.first.empty?
          x.clear
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
        @next = top = unique_lbl()
        @break = bot = unique_lbl()
        @redo = unique_lbl()
        set_label top
        process x.shift
        add "#{cond} #{bot}"
        set_label @redo
        process x.shift
        goto top
        set_label bot
        
        restore_condmod(*cm)
      end
      
      def process_until(x)
        process_while(x, "git")
      end
      
      def process_loop(x)
        b = @break
        @break = unique_lbl()
        top = unique_lbl()
        set_label top
        process(x.shift)
        goto top
        set_label @break
        @break = b
      end
      
      def process_break(x)
        if @break
          goto @break
        else
          raise "Unable to handle this break."
        end
      end
            
      def process_redo(x)
        if @redo
          goto @redo
        else
          raise "unable to handle this redo."
        end
      end
      
      def generate_when(w, nxt, post)
        w[1].shift # Remove the :array
        lst = w[1].pop
        body = nil
        unless w[1].empty?
          body = unique_lbl()
          w[1].each do |cond|
            add "dup"
            process cond
            add "send === 1"
            git body
          end
        end
        add "dup"
        process lst
        add "send === 1"
        gif nxt
        add "#{body}:" if body
        process w[2]
        add "goto #{post}" if post
      end
      
      def process_case(x)
        recv = x.shift
        whns = x.shift
        els = x.shift
        
        lbls = []
        (whns.size - 1).times { lbls << unique_lbl() }
        if els
          els_lbl = unique_lbl()
          lbls << els_lbl
        else
          els_lbl = nil
        end
        post = unique_lbl()
        lbls << post
        
        single = (whns.size == 1 and !els)
        
        # nxt = post

        process recv
                
        cur = nil
        w = whns.shift
        generate_when w, lbls[0], single ? nil : post
        
        lst = whns.pop
        
        whns.each do |w|
          cur = lbls.shift
          add "#{cur}:"
          generate_when w, lbls[1], post
        end
        
        if lst
          add "#{lbls.shift}:"
          if els_lbl
            generate_when lst, els_lbl, post
          else
            generate_when lst, post, nil
          end
        end
                
        if els
          add "#{lbls.shift}:"
          process(els)
        end
        
        add "#{post}:"
        add "swap"
        add "pop"
      end
      
      def process_lasgn(x)
        name = x.shift
        idx = x.shift
        if val = x.shift
          process val
        end
        add "set #{name}:#{idx}"
      end

      def process_op_asgn_or(x)
        process x.shift #lvar
        lbl = unique_lbl()
        git lbl
        process x.shift #rhs
        set_label lbl
      end

      def process_op_asgn_and(x)
        # x &&= 7
        # yields:
        # [:op_asgn_and, [:lvar, :x, 2], [:lasgn, :x, 2, [:lit, 7]]]]
        process x.shift #lvar
        lbl = unique_lbl()
        gif lbl
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
        lbl = unique_lbl()
        msg = x.shift
        operator = x.shift # :and or :or
        msg2 = x.shift #assignment
        arg = x.shift
        process(lvar.dup)
        add "send #{msg}"
        (operator == :or) ? git(lbl) : gif(lbl)
        x.unshift [:array, arg]
        x.unshift msg2
        x.unshift lvar
        x.unshift :attrasgn
        process x
        set_label lbl
      end
      
      def process_lvar(x)
        name = x.shift
        idx = x.shift
        add "push #{name}:#{idx}"
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
        y = x.reverse
        while i = y.shift
          process i
        end
        x.clear
        
        add "make_hash #{sz}"
      end
      
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
        process x.shift
        body = x.shift
        add "open_metaclass"
        add "dup"
        meth = @compiler.compile_as_method body, :__metaclass_init__
        idx = @method.add_literal meth
        add "push_literal #{idx}"
        add "swap"
        add "attach __metaclass_init__"
        add "pop"
        add "send __metaclass_init__"
      end

      def process_class(x)
        name = x.shift
        sup = x.shift
        body = x.shift
        
        # add "push_encloser"
        
        if name.size == 2
          under = nil
        else
          under = name[1]
          process under
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
        meth.assembly = "push self\nset_encloser\n" + meth.assembly
        add "push_literal #{idx}"
        add "swap"
        add "attach __class_init__"
        add "pop"
        add "send __class_init__"
        add "pop"
        add "push_encloser"
        # add "set_encloser"
      end
      
      def process_module(x)
        name = x.shift
        body = x.shift
        
        # add "push_encloser"
        
        if name.size == 2
          under = nil
        else
          under = name[1]
          process under
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
        meth.assembly = "push self\nset_encloser\n" + meth.assembly
        add "push_literal #{idx}"
        add "swap"
        add "attach __module_init__"
        add "pop"
        add "send __module_init__"
        add "pop"
        add "push_encloser"
        # add "set_encloser"
      end
      
      def process_begin(x)
        b = @break
        @break = unique_lbl()
        process x.shift
        set_label @break
        @break = b
      end

      def process_rescue(x)
        ex = unique_exc()
        fin = unique_lbl()
        rr = unique_lbl()
        old_retry = @retry_label
        @retry_label = unique_lbl()
        set_label @retry_label
        body = x.shift
        res = x.shift
        
        add "#exc_start #{ex}"
        process body
        # If we reach here, then no exception has been thrown, so
        # we skip all the code that checks exceptions, etc.
        goto fin
        add "#exceptions #{ex}"
        do_resbody res, rr, fin
        add "#{rr}:"
        add "push_exception"
        add "raise_exc"
        add "#{fin}:"
        # Since this is always the end if the exception has either
        # not occured or has been correctly handled, we clear the current
        # exception. There is an optimization that could be done here,
        # by setting up the jumps so that we only execute the clear
        # when we actually ran any exception handling code. But it doesn't
        # hurt to always do it for now.
        add "clear_exception"
        add "#exc_end #{ex}"
        @retry_label = old_retry 
      end

      def process_retry(x)
        raise "'retry' called outside of a begin/end block" unless @retry_label
        goto @retry_label
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

        static_nodes = [:self, :nil, :true, :false, :lit, :lasgn, :gasgn, :iasgn, 
          :cdecl, :cvdecl, :cvasgn, :lvar, :str, :array, :hash]
        node = expr.shift
        case node
          when *static_nodes
            add "push true"
          when :call
            node_type = expr[0].first
            if (node_type != :self && node_type != :const) || expr.last.size > 1
              return(reject_defined) 
            end
            receiver = expr.shift # self or a const
            msg = expr.shift # method name

            if receiver[0] == :self
              add "push :#{msg}"
              add "push self"
              add "send respond_to? 1"
            else
              lbl = unique_lbl()
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
            end
          when :cvar
            STDERR.puts "WARNING: 'define?' does not yet handle class variables."
            add "push false"
          when :gvar
            add "push :#{expr.shift}"
            add "push Globals"
            add "send key? 1"
          when :ivar
            lit = @method.add_literal(expr.shift)
            add "push_literal #{lit}"
            add "push self"
            add "send instance_variables 0"
            add "send include? 1"
          when :yield
            add "send_primitive block_given"
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
        STDERR.puts "Passed a complex expression to 'defined?'. This is why we can't have nice things."
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
          cond = [[:const, :RuntimeError]]
        else
          cond.shift
        end
        
        if other.nil?
          nxt = rr
        else
          nxt = unique_lbl()
        end
        
        one_cond = (cond.size == 1)
        
        bl = unique_lbl()
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
        
        if !one_cond
          add "#{bl}:"
        end
        
        process body
        goto fin
        
        if !other.nil?
          add "#{nxt}:"
          do_resbody other, rr, fin
        end
      end
      
      def process_ensure(x)
        ex = unique_exc()
        add "#exc_start #{ex}"
        process x.shift
        # We don't jump past the exceptions code here like we did
        # with a rescue because this code needs to be run no matter
        # what.
        add "#exceptions #{ex}"
        process x.shift
        add "push_exception"
        lbl = unique_lbl()
        gif lbl
        add "push_exception"
        add "raise_exc"
        set_label lbl
        add "#exc_end #{ex}"
      end
      
      def process_return(x)
        val = x.shift
        if val
          process val
        else
          add "push nil"
        end
        add "ret"
      end
      
      def process_masgn(x)
        rhs = x.shift
        splat = x.shift
        source = x.shift

        if source
          # The sexp has 2 nodes that do the same thing. It's annoying.
          if source[0] == :to_ary or source[0] == :splat
            process source.last
            add "cast_tuple"
          elsif source[0] == :array
            handle_array_masgn rhs, splat, source
            return
          else
            process source
          end
        end
        
        rhs.shift # get rid of :array
        rhs.each do |part|
          add "unshift_tuple"
          process part
          add "pop"
        end
        
        if splat
          add "cast_array"
          process splat
          add "pop"
        end
      end
      
      def handle_array_masgn(rhs, splat, source)        
        rhs.shift
        source.shift
        
        if rhs.size > source.size
          raise "Too many vars on rhs."
        elsif source.size > rhs.size
          raise "Too many vars in source."
        elsif splat
          raise "splat is stupid."
        end
        
        source.zip(rhs).each do |k, v|
          process k
          process v
          add "pop"
        end
      end
      
      def detect_primitive(body)
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
        if cl.kind_of?(Array) and cl[0,3] == [:call, [:const, :Ruby], kind]
          args = cl.last
          args.shift
          if args.size == 1
            ary = args.last
            if ary.kind_of? Array and [:lit, :str].include?(ary[0])
              return ary.last
            end
          end
        end
        
        return nil
      end
      
      def process_defs(x)
        s = @output
        str = ""
        @output = str
        process(x.shift)
        @output = s
        process_defn x, "attach_method", str
      end
      
      def process_defn(x, kind="add_method", recv="push_self")
        name = x.shift
        args = x.shift
        body = x.shift
        
        # Log.debug " ==> Compiling '#{name}'"
        
        prim = detect_primitive(body)
        state = RsLocalState.new
        
        defaults = args[4]

        args[1].each { |e| state.local(e) }          
        if args[3]
          state.local args[3].first
        end
        if defaults
          defaults.shift
          defaults.each do |var|
            state.local var[1]
          end
        end
        
        meth = nil
        @compiler.as_method_body(name) do
          meth = @compiler.compile_as_method body, name, state
        end
        
        meth.primitive = prim if prim
        idx = @method.add_literal meth
        add "push_literal #{idx}"
        add recv
        add "#{kind} #{name}"
        str = ""
        required = args[1].size
        args[1].each do |var|
          str << "set #{var}:#{state.local(var)}\npop\n"
        end
        
        max = min = args[1].size
        
        ba_name = nil
        if args.last and args.last.first == :block_arg
          ba = args.last
          ba_name = ba[1]
          ba_idx = state.local(ba_name)
        end
        
        if defaults
          idx = min
          defaults.each do |var|
            lbl = "set#{state.unique_id}"
            str << "passed_arg #{idx}\ngit #{lbl}\n"
            save = @output
            @output = ""
            cur = @method
            @method = meth
            process var.last
            str << @output
            @output = save
            @method = cur
            idx += 1
            str << "#{lbl}:\nset #{var[1]}:#{state.local(var[1])}\npop\n"
          end
          
          max = idx
        end
        splat = args[3]
        if splat
          str << "make_rest #{required}\nset #{splat.first}\npop\n"
          max = 0
          req = -1
        else
          req = min
        end
        
        if ba_name
          str << "push_block\npush Proc\nsend from_environment 1\nset #{ba_name}:#{ba_idx}\npop\n"
        end
        
        meth.required = req
        
        str = "check_argcount #{min} #{max}\n" + str
        
        meth.assembly = str + meth.assembly
      end
      
      def detect_class_special(x)
        recv = x.shift
        meth = x.shift
        args = x.shift
        if recv == [:self] and meth == :ivar_as_index
          args.shift
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
        end
        return false
      end
      
      def process_call(x, block=false)
        
        x.unshift :call
        asm = detect_special(:asm, x)
        if asm
          add asm
          x.clear
          return
        end
        
        #p x
        x.shift
        #p x
        
        if @compiler.in_class_body
          out = detect_class_special(x.dup)
          if out
            x.clear
            return
          end
        end
        
        recv = x.shift
        meth = x.shift
        args = x.shift
        
        if meth == :block_given?
          add "send_primitive block_given"
          return
        end
        
        grab_args = false
        
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
          sz = nil
        end
        
        if block
          @post_send = ps = unique_lbl()
        end
        
        if grab_args
          add "get_args"
        end
        
        if block
          op = "&send"
          process block
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
        add "#{op} #{meth} #{sz}"
        add "#{ps}:" if block
      end
      
      def process_super(x, block=false)
        args = x.shift
        if args
          args.shift
          args.reverse.each { |a| process(a) }
          sz = args.size
        else
          sz = 0
        end
        
        if block
          @post_send = ps = unique_lbl()
        end
        
        if block
          process block
        else
          add "push nil"
        end
        
        add "super #{@method.name} #{sz}"
        add "#{ps}:" if block
      end
      
      def process_attrasgn(x)
        process_call x
      end
      
      def process_block_pass(x)
        blk = x.shift
        cl = x.shift
        cl.shift
        process_call cl, blk
      end
      
      def process_iter(x)
        cl = x.shift
        args = x.shift
        body = x.shift
        kind = cl.shift
        iter = [:block_iter, args, body]
        if kind == :call
          process_call cl, iter
        else
          process_super cl, iter
        end
      end
      
      def process_block_iter(x)
        one = unique_lbl()
        two = unique_lbl()
        add "push &#{@post_send}"
        add "push &#{one}"
        add "push_context"
        add "send_primitive create_block"
        goto two
        process x.shift
        process x.shift
        add "#{one}: soft_return"
        set_label two
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
        add "send to_s 0"
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
      
      def process_yield(x)
        args = x.shift
        
        if args
          kind = args.first
          if kind == :array
            args.shift
            args.reverse.each { |a| process(a) }
            sz = args.size
          else
            process(args)
            sz = 1
          end
        else
          sz = 0
        end
        
        x.shift
        
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
        else
          add "soft_return"
        end
      end
      
      def process_newline(x)
        line = x.shift
        @method.file = x.shift
        add "\#line #{line}"
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
      
    end
  end
end
