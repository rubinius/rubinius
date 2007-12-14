module Compiler1
class Bytecode::Compiler
  class Plugin
    
    def self.flag(n=nil)
      if n
        @flag = n
      else
        @flag
      end
    end

    def initialize(compiler)
      @compiler = compiler
    end
    
    def flag?(val)
      @compiler.flag?(val)
    end
    
    def process(what)
      @compiler.process(what)
    end
    
    def add(what)
      @compiler.add(what)
    end
    
    def capture(&b)
      @compiler.capture(&b)
    end
    
    def unique_lbl(arg)
      @compiler.unique_lbl(arg)
    end
    
    def unique_exc
      @compiler.unique_exc
    end
    
    def temp
      name = "+temp_#{@compiler.unique_id}"
      
    end
    
    def run(recv, meth, args, block)
      return false unless @compiler.flag?(self.class.flag)
      handle(recv, meth, args, block)
    end
  end
end

class MetaOperatorPlugin < Compiler1::Bytecode::Compiler::Plugin
  
  flag :fast_math
  
  MetaMath = {
    :+ =>   "meta_send_op_plus",
    :- =>   "meta_send_op_minus", 
    :== =>  "meta_send_op_equal",
    :"!=" =>  "meta_send_op_nequal",
    :=== => "meta_send_op_tequal",
    :< =>   "meta_send_op_lt", 
    :> =>   "meta_send_op_gt"
  }
  
  def handle(recv, meth, args, block)
    return false if block
    
    name = MetaMath[meth]
    if name and args and args.size == 2
      process args.last
      process recv
      add name
      return true
    end
    
    return false
  end
  
end

class SystemMethodPlugin < Compiler1::Bytecode::Compiler::Plugin
  
  flag :fast_system
  
  Methods = {
    :kind_of? => "kind_of",
    :is_a? => "kind_of",
    :instance_of? => "instance_of",
    :nil? => "is_nil",
    :equal? => "equal",
    :class => "class"
  }
  
  # How many arguments each method takes.
  Args = {
    :kind_of?     => 1,
    :is_a?        => 1,
    :instance_of? => 1,
    :nil?         => 0,
    :equal?       => 1,
    :class        => 0
  }
  
  # How to map class checks directly to instructions.
  # I highly doubt anyone does the last one, but it's here
  # for completeness.
  KindOf = {
    :Fixnum =>    "is_fixnum",
    :Symbol =>    "is_symbol",
    :NilClass =>  "is_nil"
  }
  
  def handle(recv, meth, args, block)
    return false if block
    
    name = Methods[meth]
    
    return false unless name and args
    
    # Check that the arg count is correct for this method.
    return false unless Args[meth] == args.size - 1
    
    if args.size == 1    
      process recv
      add name
      return true
    end
    
    if args.size == 2
      cls = args.last
      # Special case for obj.kind_of?(Fixnum)
      if meth == :kind_of? and cls.first == :const
        if op = KindOf[cls.last]
          process recv
          add op
          return true
        end
      end

      process cls
      process recv
      add name
      return true
    end
    
    return false
  end
end

class NamedSendPlugin < Compiler1::Bytecode::Compiler::Plugin
  
  flag :fast_send_method
  
  def handle(recv, meth, args, block)
    return false if block
    return false unless meth == :__send__
    
    args.shift
    name = args.shift
    return false unless name
    
    count = args.size
    args.reverse.each do |a|
      process a
    end
    
    process recv
    add "push nil"
    process name
    add "dup"
    add "is_symbol"
    lbl = unique_lbl("send_")
    add "git #{lbl}"
    add "send to_sym 0"
    add "#{lbl}:"
    add "push #{count}"
    add "set_args"
    add "send_off_stack"
    return true
    
  end
end

class FieldAccessPlugin < Compiler1::Bytecode::Compiler::Plugin
  flag :fast_field_access
  
  def push_index(idx)
    process idx
    add "dup"
    add "is_fixnum"
    lbl = unique_lbl("ffa_")
    add "git #{lbl}"
    add "send to_int 0"
    add "#{lbl}:"
  end
  
  def handle(recv, meth, args, block)
    if meth == :__at__
      return false unless args.size == 2
      
      process recv
      push_index args[1]
      add "fetch_field"
      return true
    elsif meth == :__put__
      return false unless args.size == 3
      process recv
      process args[2]
      push_index args[1]
      add "store_field"
      return true
    end
    
    return false
  end
end

class TypeCoercePlugin < Compiler1::Bytecode::Compiler::Plugin
  flag :fast_coerce
  
  def handle_fixnum(recv, meth, obj)
    process obj
    add "dup"
    
    add "is_fixnum"
    lbl = unique_lbl("type_")
    add "git #{lbl}"
    add "dup"
    
    ec = unique_exc()
    add "#exc_start #{ec}"
    add "send #{meth} 0"
    after = unique_lbl("type_")
    add "goto #{after}"
    add "#exceptions #{ec}"
    add "push Fixnum"
    add "push_exception"
    add "push Type"
    add "send coerce_unable 3"
    add "#exc_end #{ec}"
    
    add "#{after}:"
    
    add "dup"
    add "is_fixnum"
    lbl2 = unique_lbl("type_")
    add "git #{lbl2}"
    add "pop"
    add "push Fixnum"
    add "push Type"
    add "send coerce_failed 2"
    add "#{lbl2}:"
    add "swap"
    add "pop"
    add "#{lbl}:"
  end
  
  def handle(recv, meth, args, block)
    if recv == [:const, :Type] and meth == :coerce_to
      # Further verify the form of the expression
      return false unless args[3].first == :lit
      return false unless args[2].first == :const
      
      meth = args[3].last
      
      return false unless meth.kind_of? Symbol
      
      cls =  args[2].last
      
      # Special case for getting a Fixnum
      if cls == :Fixnum
        handle_fixnum(recv, meth, args[1])
        return true
      end
      
      # Put the thing we're testing on the stack.
      process args[1]
      add "dup"
      
      # step 1, see if it's already the type we want.
      add "push #{cls}"
      add "swap"
      lbl = unique_lbl("type_")
      add "kind_of"
      
      # if so, we're done.
      add "git #{lbl}"
      
      
      # step 2. convert it with a send.
      # save a copy of the original, we might still need it.
      add "dup"
      
      ec = unique_exc()
      add "#exc_start #{ec}"
      add "send #{meth} 0"
      after = unique_lbl("type_")
      add "goto #{after}"
      add "#exceptions #{ec}"
      add "push #{cls}"
      add "push_exception"
      add "push Type"
      add "send coerce_unable 3"
      add "#exc_end #{ec}"
      
      add "#{after}:"
      # step 3. check it's what we wanted now.
      add "dup"
      add "push #{cls}"
      add "swap"
      add "kind_of"
      lbl2 = unique_lbl("type_")      
      add "git #{lbl2}"
      
      # step 4. conversion failed, call out to raise an error.
      
      # get rid of the bad object.
      add "pop"
      add "push #{cls}"
      add "push Type"
      add "send coerce_failed 2"
      
      add "#{lbl2}:"
      # get rid of the extra copy of the original.
      add "swap"
      add "pop"
      add "#{lbl}:"
      return true
    end
    
    return false
  end
end

=begin

class FastTimesPlugin < Compiler1::Bytecode::Compiler::Plugin
  
  flag :fast_times
  
  UnrollThreshold = 5
  
  def handle(recv, meth, args, block)
    # Only do this if you're sending times to a literal number.
    if meth != :times or recv.first := :lit and !recv.last.kind_of?(Fixnum)
      return false
    end
    
    count = recv.last
    if count < UnrollThreshold
      code = capture do
        process block
      end
      
      count.times do
        add code
      end
    end
    
    return false
  end
end


this is premature

class FastSplatAccess < Compiler1::Bytecode::Compiler::Plugin
  def process(recv, meth, args)
    return false if meth != :[] or args.size != 2
    return false if recv.first != :lvar
    
    lv = recv[2]    
    
    return false unless lv.magic == :splat
    return false if lv.assigned? or lv.passed_as_argument?
    
    if 
    return false if     
  end
end
=end

end