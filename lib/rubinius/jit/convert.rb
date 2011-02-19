require 'rubinius/jit/operations'
require 'rubinius/jit/analysis'

module Rubinius::JITCompiler
  class BasicBlock
    def initialize(ip, sp=nil)
      @ip = ip
      @sp = sp
      @instructions = []
      @visited = false
    end

    def <<(inst)
      @instructions << inst
    end

    def visit!
      @visited = true
    end

    attr_accessor :sp
    attr_reader :ip, :instructions, :visited
  end

  class BytecodeInstruction < Rubinius::CompiledMethod::Instruction
    attr_accessor :sp

    def stack_difference
      return instruction.stack_difference unless instruction.variable_stack?
      use = instruction.stack_consumed
      if use.kind_of? Array
        use = args[use[1] - 1] + use[0]
      end

      pro = instruction.stack_produced
      if pro.kind_of? Array
        pro = (args[pro[1] - 1] * pro[2]) + pro[0]
      end

      pro - use
    end

    def to_s
      str = super
      str << (' ' * (50 - str.size))
      str << " # sp=#{@sp.inspect}"
      str
    end
  end

  class Converter
    def initialize(cm)
      @cm = cm
      @blocks = {}
    end

    def add_block(ip)
      return if @blocks.key?(ip)
      @blocks[ip] = BasicBlock.new(ip)
    end

    def decode
      require 'compiler/iseq'

      decoder = Rubinius::InstructionDecoder.new(@cm.iseq)
      stream = decoder.decode(false)
      ip = 0

      stream.map do |inst|
        instruct = BytecodeInstruction.new(inst, @cm, ip)
        ip += instruct.size
        instruct
      end
    end

    def convert
      ins = decode()

      @blocks[0] = BasicBlock.new(0, -1)
      detect_goto_targets ins
      construct_blocks ins
      compute_stack ins
      create_ir
    end

    def show_blocks
      @blocks.keys.sort.each do |i|
        blk = @blocks[i]
        puts "BLOCK #{i} :: sp=#{blk.sp} :: #{blk.visited ? 'live' : 'dead'}"
        puts blk.instructions
      end
    end

    def construct_blocks(ins)
      current_block = nil

      ins.each do |i|
        if blk = @blocks[i.ip]
          current_block = blk
        end

        current_block << i
      end
    end

    def detect_goto_targets(ins)
      ins.each do |i|
        case i.opcode
        when :goto, :goto_if_true, :goto_if_false
          add_block i.args[0]
          add_block i.ip + 2
        end
      end
    end

    def compute_stack(ins)
      process = [@blocks[0]]

      until process.empty?
        blk = process.shift
        blk.visit!

        sp = blk.sp
        blk.instructions.each do |x|
          x.sp = sp
          sp += x.stack_difference
        end

        term = blk.instructions.last
        next_ip = term.ip + term.args.size + 1

        case term.opcode
        when :goto, :goto_if_true, :goto_if_false
          dest = @blocks[term.args[0]]
          if dest.sp and dest.sp != sp
            puts "Invalid stack movement detected #{dest.sp} != #{sp}"
          end

          dest.sp = sp
          if dest.ip > term.ip
            process.push dest
          end

          case term.opcode
          when :goto_if_true, :goto_if_false
            blk = @blocks[next_ip]
            blk.sp = sp
            process.push blk
          end
        when :ret
          # nothing
        else
          blk = @blocks[next_ip]
          blk.sp = sp
          process.push blk
        end
      end
    end

    class IRBasicBlock
      def initialize(ip)
        @ip = ip
        @first = nil
        @last = nil
        @users = []
      end

      attr_reader :ip, :users

      def <<(ir)
        ir.block = self

        @last.append ir if @last

        @last = ir

        @first = ir unless @first

        self
      end

      def terminator
        return @last if @last.kind_of? Terminator
        return nil
      end

      def add_user(block)
        @users << block unless @users.include?(block)
      end

      def at(idx)
        node = @first
        idx.times do
          if x = node.next
            node = x
          else
            return nil
          end
        end

        return node
      end

      def each
        node = @first

        while node
          yield node
          node = node.next
        end

        self
      end

      def instructions
        ary = []
        each { |x| ary << x }
        ary
      end

      def loop_enter
        @users.find { |i| i.ip < @ip }
      end

      def loop_body
        @users.find { |i| i.ip > @ip }
      end

      def loop_header?
        return false unless @users.size == 2
        a = loop_enter
        b = loop_body

        return true if a and b and a != b
        return false
      end

      def dominates?(blk)
        return false if blk.equal?(self)
        work = [blk]

        checked = []

        until work.empty?
          check = work.pop
          checked << check

          check.users.each do |b|
            return true if b.equal?(self)
            work.push b unless checked.include?(b)
          end
        end

        return false
      end

    end

    def create_ir
      @ir_blocks = {}

      @blocks.each do |ip, blk|
        if blk.visited
          @ir_blocks[ip] = IRBasicBlock.new(ip)
        end
      end

      ordered_keys = @ir_blocks.keys.sort

      ordered_keys.each_with_index do |ip,idx|
        byteblk = @blocks[ip]

        blk = @ir_blocks[ip]

        blk << SyncPosition.new(ip)

        byteblk.instructions.each do |i|
          case i.opcode
          when :meta_push_0
            blk << SetSimple.new(i.sp+1, 0)
          when :meta_push_1
            blk << SetSimple.new(i.sp+1, 1)
          when :push_nil
            blk << SetSimple.new(i.sp+1, nil)
          when :set_local
            blk << SetLocal.new(i.sp, i.args[0])
          when :push_local
            blk << GetLocal.new(i.sp+1, i.args[0])
          when :pop
            # noop
          when :push_literal
            lit = i.args[0] 
            case lit
            when Fixnum
              blk << SetSimple.new(i.sp+1, lit)
            when Float
              blk << SetDouble.new(i.sp+1, lit)
            else
              blk << GetLiteral.new(i.sp+1, i.args[0])
            end
          when :meta_send_op_lt
            blk << SyncPosition.new(i.ip)
            blk << Send.new(:<, i.sp-1, 1)
          when :meta_send_op_plus
            blk << SyncPosition.new(i.ip)
            blk << Send.new(:+, i.sp-1, 1)
          when :push_self
            blk << SetSelf.new(i.sp+1)
          when :allow_private, :check_interrupts
            # skip
          when :send_stack
            blk << SyncPosition.new(i.ip)
            blk << Send.new(i.args[0], i.sp-i.args[1], i.args[1])
          when :goto_if_false
            dest = @ir_blocks[i.args[0]]
            dest.add_user blk
            else_block = @ir_blocks[ordered_keys[idx+1]]
            else_block.add_user blk

            blk << GotoIfFalse.new(i.sp, dest, else_block)
          when :goto
            dest = @ir_blocks[i.args[0]]
            dest.add_user blk
            blk << Goto.new(dest)
            break # the rest of the instructions in this block are unreachable
          when :ret
            blk << Return.new(i.sp)
            break # the rest of the instructions in this block are unreachable
          end
        end
      end

      ordered_keys = @ir_blocks.keys.sort

      ordered_keys.each_with_index do |ip, idx|
        blk = @ir_blocks[ip]

        unless blk.terminator
          dest = @ir_blocks[ordered_keys[idx+1]]
          unless dest
            raise "weird"
          end
          blk << Goto.new(dest)
        end
      end
    end

    def show_ir
      @ir_blocks.keys.sort.each do |ip|
        blk = @ir_blocks[ip]

        if blk.users
          puts "#{ip}: # users=#{blk.users.map { |i| i.ip }.inspect}"
        else
          puts "#{ip}:"
        end
        blk.each do |i|
          puts "  #{i.asm}"
        end
      end
    end

    def optimize(show_changes=false)
      pc = Analysis::PassCollection.new(show_changes)
      pc << Analysis::RedundentLoad.new
      pc << Analysis::LocalInfo.new
      pc << Analysis::MathLocals.new(true)

      pc.run @ir_blocks
    end
  end
end
