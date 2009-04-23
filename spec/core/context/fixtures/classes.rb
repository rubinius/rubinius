module ContextSpecs

  class A
    @cm = def ret_7(call_block=false, call_method=false)
      # Yields to allow context manipulation
      Rubinius.asm { push :nil; yield_debugger }
      if call_block
        b = [1,2,3].each {|a| a*2; @called_block = true }
      end
      if call_method
        called_from_ret_7
      end
      7
    end

    def self.ret_7_cm
      @cm
    end

    # Need to do this because the specs modify the instruction sequence
    @orig_bytecodes = @cm.iseq.dup
    @encoder = Rubinius::InstructionSequence::Encoder.new

    def self.orig_bytecodes
      @orig_bytecodes
    end

    def self.encoder
      @encoder
    end

    def self.describe
      puts "ret_7 bytecode:"
      puts @cm.decode
      puts "Static scope: #{@cm.staticscope.module}  #{@cm.staticscope.parent}"
      puts ""

      #blk = @cm.literals.at(0)
      #puts "block bytecode:"
      #puts blk.decode
      #puts "Static scope: #{blk.staticscope.module} #{blk.staticscope.parent}"
    end

    #describe

    def change_ret(i)
      bc = A.ret_7_cm.iseq.decode
      bc[-2][1] = i
      A.encoder.encode_stream(bc)
    end

    def call_ret_7(call_block=false, call_method=false)
      ret_7(call_block, call_method)
    end

    def called_from_ret_7
      @called_from_ret_7 = true
    end

    def called_block?
      @called_block
    end

    def called_from_ret_7?
      @called_from_ret_7
    end
  end

  class B
    def inspect_stack(depth)
      ctx = Rubinius::Task.current.current_context
      #puts ctx.method.decode
      inspector 'First', :second, 3, [4, 5, 6], ctx.stack_at(depth)
    end

    def inspector(*args)
      args.last
    end
  end

  # Simple listener class to receive call-backs when yield_debugger is hit
  class Listener
    def initialize
      @debug_channel = Channel.new
      @msg_channel = Channel.new
      Rubinius::VM.debug_channel = @debug_channel
    end

    def wait_for_breakpoint(&prc)
      @thread = Thread.new do
        @msg_channel.send true
        thr = @debug_channel.receive
        prc.call thr.task.current_context if block_given?
        thr.control_channel.send nil
      end
      @msg_channel.receive
      Thread.pass until @thread.status == "sleep"
    end
  end

end
