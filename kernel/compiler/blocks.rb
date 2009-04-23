class Compiler
  class BlockExtractor
    class Block
      def initialize(ip, block_id)
        @ip = ip
        @block_id = block_id
        @instructions = []
        @next = []
      end

      attr_reader :ip, :block_id, :instructions

      def <<(insn)
        @instructions << insn
      end

      def add_next(block, reason=:goto)
        @next << [block, reason]
      end

      def next_blocks
        @next
      end

      def size
        @instructions.size
      end
    end

    def initialize(iseq)
      @iseq = iseq.opcodes
      @block_id = 0
      @blocks = Hash.new { |h,k| h[k] = Block.new(k, @block_id += 1) }
    end

    def run
      run_from(0)
    end

    def run_from(start_ip)
      if block = @blocks.key?(start_ip)
        return @blocks[start_ip]
      end

      block = @blocks[start_ip]

      ip = start_ip
      total = @iseq.size

      while ip < total
        opcode = Rubinius::InstructionSet[@iseq[ip]]
        size = opcode.size

        if size == 1
          block << opcode.opcode
        else
          ary = [opcode.opcode]
          case size
          when 2
            ary << @iseq[ip + 1]
          when 3
            ary << @iseq[ip + 1]
            ary << @iseq[ip + 2]
          end
          block << ary
        end

        case opcode.opcode
        when :setup_unwind, :goto_if_false, :goto_if_true, :goto_if_defined
          dest = @iseq[ip + 1]
          block.add_next run_from(dest), :condition
          block.add_next run_from(ip + 2)
          return block
        when :goto
          dest = @iseq[ip + 1]
          block.add_next run_from(dest)
          return block
        when :ret, :raise_exc
          return block
        end

        ip += size
      end

      raise "control flowed off end"
    end

    class BlockInspector
      def initialize(&block)
        @seen_blocks = {}
        @operation = block
      end

      def run(block)
        return if @seen_blocks[block.block_id]
        @seen_blocks[block.block_id] = true

        @operation.call block

        block.next_blocks.each do |sub, reason|
          run sub
        end
      end
    end
  end
end
