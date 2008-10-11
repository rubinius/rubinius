module BreakpointSpecs

  class Debuggee
    cm = def simple_method
      a = 10
      b = 45
      c = a * b
      puts "#{a} * #{b} = #{c}"
    end

    # Need to do this because the specs modify the instruction sequence
    @orig_bytecodes = cm.iseq.dup

    def self.orig_bytecodes
      @orig_bytecodes
    end

    sm_cm = def step_method
      a = 10
      tot = 0
      1.upto(a) do |i|
        tot += a * i
        break if tot > 50
      end
      puts "Total is #{tot}"
    end

    if_cm = def if_method
      if false
        'f'
      else
        't'
        'ohoh'
      end
    end
  end

  class TaskStub
    def initialize(ctxt)
      @current_context = ctxt
    end
    attr_accessor :current_context, :stack_value

    def get_stack_value(depth)
      @stack_value
    end
  end

  class ContextStub
    def initialize(cm, ip)
      @method = cm
      @ip = ip
    end

    attr_accessor :method, :ip, :sender

    def line
      @method.line_from_ip(@ip - 1)
    end

    def file
      @method.file
    end
  end
end

=begin

     ### Bytecode for step_method ###

           # line 18
     0000: check_argcount         0, 0
           # line 19
     0003: push_int               10
     0005: set_local              :a
     0007: pop
           # line 20
     0008: meta_push_0
     0009: set_local              :tot
     0011: pop
           # line 21
     0012: push_local             :a
     0014: push_literal           #<CompiledMethod:0x40045 name=__block__ file=spec/core/breakpoint/fixtures/classes.rb>
     0016: create_block2
     0017: meta_push_1
     0018: push_cpath_top
     0019: find_const             :LongReturnException
     0021: send_method            :allocate
     0023: set_local              :@lre
     0025: pop
     0026: send_stack_with_block  :upto, 1
     0029: goto                   50
     0031: push_exception
     0032: dup_top
     0033: push_local             :@lre
     0035: equal
     0036: goto_if_false          47
     0038: clear_exception
     0039: dup_top
     0040: send_method            :is_return
     0042: goto_if_false          48
     0044: send_method            :value
     0046: ret
     0047: raise_exc
     0048: send_method            :value
     0050: pop
           # line 25
     0051: push_local             :tot
     0053: set_call_flags         1
     0055: send_method            :to_s
     0057: push_literal           "Total is "
     0059: string_dup
     0060: string_append
     0061: push_self
     0062: set_call_flags         1
     0064: send_stack             :puts, 1
     0067: sret


     ### Bytecode for upto block in step_method ###

            # line 21
     0000:  cast_for_single_block_arg
     0001:  set_local_depth            0, :i
     0004:  pop
            # line 22
     0005:  push_local_depth           0, :i
     0008:  push_local                 0
     0010:  send_stack                 :*, 1
     0013:  push_local                 1
     0015:  meta_send_op_plus
     0016:  set_local                  1
     0018:  pop
            # line 23
     0019:  push_int                   50
     0021:  push_local                 1
     0023:  meta_send_op_gt
     0024:  goto_if_false              38
     0026:  push_nil
     0027:  push_local                 2
     0029:  send_stack                 :"break_value=", 1
     0032:  pop
     0033:  push_local                 2
     0035:  raise_exc
     0036:  goto                       39
     0038:  push_nil
     0039:  soft_return


     ### Bytecode for if_method ###

            # line 28
     0000:  check_argcount             0, 0
            # line 29
     0003:  push_false
     0004:  goto_if_false              11
            # line 30
     0006:  push_literal               "f"
     0008:  string_dup
     0009:  goto                       14
            # line 32
     0011:  push_literal               "t"
     0013:  string_dup
     0014:  pop
            # line 33
     0015:  push_literal               "ohoh"
     0017:  string_dup
     0018:  sret

=end
