require 'sexp/processor'
require 'sydparse'
require 'translation/normalize'
require 'translation/local_scoping'
require 'sexp/composite_processor'
require 'bytecode/assembler'

module Bytecode
  
  class MethodDescription
    def initialize(name)
      @name = name
      @assembly = ""
      @literals = []
    end
    
    def add_literal(obj)
      idx = @literals.size
      @literals << obj
      return idx
    end
    
    attr_accessor :name, :assembly, :literals
    
    def to_cmethod
      asm = Bytecode::Assembler.new
      stream = asm.assemble @assembly
      enc = Bytecode::InstructionEncoder.new
      bc = enc.encode_stream stream
      lcls = asm.number_of_locals
      cmeth = Rubinius::CompiledMethod.from_string bc, lcls
      cmeth.literals = encode_literals()
      return cmeth
    end
    
    def encode_literals
      tup = Rubinius::Tuple.new(@literals.size)
      i = 0
      @literals.each do |lit|
        case lit
        when Symbol
          str = Rubinius::String.new(lit.to_s)
          out = str.to_sym
        when Bytecode::MethodDescription
          out = lit.to_cmethod
        else
          raise "Unable to encode literal: #{lit.inspect}"
        end
        tup.put i, out
        i += 1
      end
      
      return tup
    end
  end
  
  class Compiler    
    def compile_as_method(sx, name)
      meth = MethodDescription.new(name)
      comp = CompositeSexpProcessor.new
      # Convert the block args to the new rules...
      comp << RsLocalScoper.new
      # normalize the rest of the sexp...
      comp << RsNormalizer.new
      # and compile it!
      pro = Processor.new(self, meth)
      comp << pro
      comp.process sx
      pro.finalize
      return meth
    end
    
    class Processor < SexpProcessor
      def initialize(cont, meth)
        super()
        self.require_expected = false
        self.strict = true
        self.auto_shift_type = true
        @compiler = cont
        @method = meth
        @output = ""
        @unique_id = 0
      end
      
      def unique_id
        @unique_id += 1
      end
      
      def unique_lbl
        "lbl#{unique_id}"
      end
      
      def unique_exc
        "exc#{unique_id}"
      end
      
      def add(str)
        @output << (str.strip + "\n")
      end
      
      def finalize
        @method.assembly = @output
        add "ret"
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
        add "gif #{lbl}"
        process x.shift
        add "#{lbl}:"
      end
      
      def process_or(x)
        process x.shift
        lbl = unique_lbl()
        add "git #{lbl}"
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
        else
          raise "Unable to handle literal '#{obj.inspect}'"
        end
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
        if thn and els
          el = unique_lbl()
          ed = unique_lbl()
          gif el
          process thn
          goto ed
          set_label el
          process els
          set_label ed
        elsif !els
          ed = unique_lbl()
          gif ed
          process thn
          set_label ed
        elsif !thn
          ed = unique_lbl()
          git ed
          process els
          set_label ed
        end
      end
      
      def process_block(x)
        while i = x.shift
          process i
        end
      end
      
      def process_scope(x)
        out = process x.shift
        x.clear
        out
      end
      
      def process_while(x, cond="gif")
        top = unique_lbl()
        bot = unique_lbl()
        set_label top
        process x.shift
        add "#{cond} #{bot}"
        process x.shift
        goto top
        set_label bot
      end
      
      def process_until(x)
        process_while(x, "git")
      end
      
      def process_lasgn(x)
        name = x.shift
        idx = x.shift
        process x.shift
        add "set #{name}:#{idx}"
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
      
      def process_hash(x)
        sz = x.size
        y = x.reverse
        while i = y.shift
          process i
        end
        x.clear
        
        add "make_hash #{sz}"
      end
      
      def process_zarray(x)
        add "make_array 0"
      end
      
      def process_argscat(x)
        ary = x.shift        
        itm = x.shift
        process itm
        add "cast_array"
        add "push_array"
        ary.shift
        ary.reverse.each do |i|
          process i
        end
      end
      
      def process_ivar(x)
        name = x.shift
        add "push #{name}"
      end
      
      def process_iasgn(x)
        name = x.shift
        process x.shift
        add "set #{name}"
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
      
      def process_to_ary(x)
        process x.shift
        add 'cast_array'
      end
      
      def process_class(x)
        name = x.shift
        sup = x.shift
        body = x.shift
        
        add "push_encloser"
        
        if name.size == 2
          under = nil
        else
          under = name[1]
          process under
        end
        
        name = name.last
        
        if sup.nil?
          add "push nil"
        else
          process sup
        end
        
        if under
          add "open_class_under #{name}"
        else
          add "open_class #{name}"
        end
        add "dup"
        
        meth = @compiler.compile_as_method body, :__class_init__
        idx = @method.add_literal meth
        meth.assembly = "push self\nset_encloser\n" + meth.assembly
        add "push_literal #{idx}"
        add "swap"
        add "attach __class_init__"
        add "send __class_init__"
        add "pop"
        add "set_encloser"
      end
      
      def process_module(x)
        name = x.shift
        body = x.shift
        
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
        
        meth = @compiler.compile_as_method body, :__module_init__
        idx = @method.add_literal meth
        meth.assembly = "push self\nset_encloser\n" + meth.assembly
        add "push_literal #{idx}"
        add "swap"
        add "attach __module_init__"
        add "send __module_init__"
      end
      
      def process_rescue(x)
        ex = unique_exc()
        fin = unique_lbl()
        rr = unique_lbl()
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
        add "raise"
        add "#{fin}:"
        # Since this is always the end if the exception has either
        # not occured or has been correctly handled, we clear the current
        # exception. There is an optimization that could be done here,
        # by setting up the jumps so that we only execute the clear
        # when we actually ran any exception handling code. But it doesn't
        # hurt to always do it for now.
        add "clear_exception"
        add "#exc_end #{ex}"
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
        add "raise"
        set_label lbl
        add "#exc_end #{ex}"
      end
      
      def process_return(x)
        process x.shift
        add "ret"
      end
      
      def process_masgn(x)
        rhs = x.shift
        splat = x.shift
        source = x.shift
        
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
        
        rhs.shift # get rid of :array
        rhs.each do |part|
          add "unshift_tuple"
          process part
        end
        
        if splat
          add "cast_array"
          process splat
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
        end
      end
      
      def process_defn(x)
        name = x.shift
        args = x.shift
        body = x.shift
                
        meth = @compiler.compile_as_method body, name
        idx = @method.add_literal meth
        add "push_self"
        add "push_literal #{idx}"
        add "add_method #{name}"
        str = ""
        required = args[1].size
        args[1].each do |var|
          str << "set #{var}\n"
        end
        
        max = min = args[1].size
        
        defaults = args[4]
        
        if defaults
          defaults.shift
          idx = min
          defaults.each do |var|
            lbl = unique_lbl()
            str << "passed_arg #{idx}\ngit #{lbl}\n"
            save = @output
            @output = ""
            process var.last
            str << @output
            @output = save
            idx += 1
            str << "#{lbl}:\nset #{var[1]}\n"
          end
          
          max = idx
        end
        splat = args[3]
        if splat
          str << "make_rest #{required}\nset #{splat.first}\n"
          max = 0
        end
        
        if args.last and args.last.first == :block_arg
          ba = args.last
          name = ba[1]
          idx = ba[2]
          str << "push_block\nset #{name}:#{idx}\n"
        end
        
        str = "check_argcount #{min} #{max}\n" + str
        
        meth.assembly = str + meth.assembly
      end
      
      def process_call(x, block=false)
        recv = x.shift
        meth = x.shift
        args = x.shift
        
        if args
          args.shift
          args.reverse.each { |a| process(a) }
        end
        
        if block
          op = "&send"
          process block
        else
          op = "send"
        end
        
        process recv
        
        if args
          sz = args.size
        else
          sz = nil
        end
        
        add "#{op} #{meth} #{sz}"        
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
        cl.shift
        iter = [:block_iter, args, body]
        process_call cl, iter
      end
      
      def process_block_iter(x)
        one = unique_lbl()
        two = unique_lbl()
        add "push &#{one}"
        add "push_context"
        add "send_primitive create_block"
        goto two
        process x.shift
        process x.shift
        add "#{one}: soft_return"
        set_label two
      end
    end
  end
end