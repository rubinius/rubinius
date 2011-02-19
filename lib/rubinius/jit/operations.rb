module Rubinius::JITCompiler
  class Operation
    def initialize
      @next = nil
      @prev = nil
      @block = nil
    end

    attr_accessor :next, :prev, :block

    def inspect
      "#<#{self.class} \"#{asm}\">"
    end

    def append(insn)
      if @next
        @next.prev = insn
      end

      insn.next = @next
      insn.prev = self

      @next = insn
    end

    def prepend(insn)
      if @prev
        @prev.next = insn
      end

      insn.prev = @prev
      insn.next = self

      @prev = insn
    end

    def replace(insn)
      if @prev
        @prev.next = insn
      end

      if @next
        @next.prev = insn
      end

      insn.next = @next
      insn.prev = @prev

      self
    end

    def remove!
      if @prev
        @prev.next = @next
      end

      if @next
        @next.prev = @prev
      end
    end

    def find_backward(obj)
      x = self

      while x
        return true if x == obj
        x = x.prev
      end

      return false
    end

    def store_search_backward(store)
      x = @prev

      while x
        return x if x.kind_of? Store and x.destination == store
        x = x.prev
      end

      nil
    end

    def find_user
      return nil unless Store

      x = @next
      while x
        if x.kind_of?(Read)
          return x if x.reads.include?(destination)
        end
      end

      return nil
    end

    def dominates?(i)
      case i
      when Operation
        return false if !@block

        if @block == i.block
          return true if i.find_backward(self)
          return false
        end

        return @block.dominates?(i.block)
      when IRBasicBlock
        return @block.dominates?(i)
      else
        false
      end
    end
  end

  class Store < Operation
    def initialize(dest)
      @destination = dest
    end

    attr_reader :destination
  end

  module Read
    def reads
      return [@source]
    end
  end

  module Terminator; end

  class Copy < Store
    include Read

    def initialize(dest, source)
      super(dest)
      @source = source
    end

    attr_reader :source

    def asm
      "$r#{@destination} = $r#{@source}"
    end
  end

  class SyncPosition < Operation
    def initialize(ip)
      @ip = ip
    end

    attr_reader :ip

    def asm
      "sync_pos #{@ip}"
    end
  end

  class SetSelf < Store
    def asm
      "$r#{@destination} = self"
    end
  end

  class SetSimple < Store
    def initialize(dest, value)
      super(dest)
      @value = value
    end

    attr_reader :value

    def asm
      "$r#{@destination} = #{@value.inspect}"
    end

    def ==(other)
      case other
      when SetSimple
        @destination == other.destination and other.value == @value
      else
        super
      end
    end
  end

  class SetDouble < Store
    def initialize(dest, value)
      super(dest)
      @value = value
    end

    attr_reader :value

    def asm
      "$r#{@destination} = #{@value}"
    end
  end

  class SetLocal < Operation
    include Read

    def initialize(source, local)
      @source = source
      @local = local
    end

    attr_reader :source, :local

    def asm
      "locals[#{@local}] = $r#{@source}"
    end
  end

  class GetLocal < Store
    def initialize(dest, local)
      super(dest)
      @local = local
    end

    attr_reader :local

    def asm
      "$r#{@destination} = locals[#{@local}]"
    end
  end

  class GetLiteral < Store
    def initialize(dest, literal)
      super(dest)
      @literal = literal
    end

    attr_reader :literal

    def asm
      "$r#{@destination} = #{literal}"
    end
  end

  class BinaryOperation < Store
    def initialize(dest, a, b)
      super(dest)
      @lhs = a
      @rhs = b
    end

    attr_reader :lhs, :rhs
  end

  class UnboxInt < Operation
    include Read

    def initialize(dest_int, source)
      @dest_int = dest_int
      @source = source
    end

    attr_reader :source, :dest_int

    def asm
      "$i#{@dest_int} = $r#{@source}"
    end
  end

  class BoxInt < Store
    def initialize(dest, source_int)
      super(dest)
      @source_int = source_int
    end

    attr_reader :source_int

    def asm
      "$r#{@destination} = $i#{@source_int}"
    end
  end

  class IntSetConstant < Operation
    def initialize(dest, value)
      @destination = dest
      @value = value
    end

    attr_reader :destination, :value

    def asm
      "$i#{@destination} = #{@value}"
    end
  end

  class IntLessThan < BinaryOperation
    def asm
      "$i#{@destination} = $i#{@lhs} < #{@rhs}"
    end
  end

  class IntAdd < BinaryOperation
    def asm
      "$i#{@destination} = $i#{@lhs} + #{@rhs}"
    end
  end

  class DoubleAdd < BinaryOperation
  end

  class GotoIfFalse < Operation
    include Read
    include Terminator

    def initialize(source, dest_block, else_block)
      @source = source
      @dest_block = dest_block
      @else_block = else_block
    end

    attr_reader :source, :dest_block, :else_block

    def asm
      "gif $r#{@source} -> #{@dest_block.ip}, else -> #{@else_block.ip}"
    end
  end

  class Goto < Operation
    include Terminator

    def initialize(dest_block)
      @dest_block = dest_block
    end

    attr_reader :dest_block

    def asm
      "goto #{@dest_block.ip}"
    end
  end

  class IntGotoEQGT < Operation
    include Terminator

    def initialize(source, val, dest_block, else_block)
      @source = source
      @value = val
      @dest_block = dest_block
      @else_block = else_block
    end

    def asm
      "goto if $i#{@source} >= #{@value} -> #{@dest_block.ip}, else -> #{@else_block.ip}"
    end
  end

  class Send < Store
    include Read

    def initialize(method, receiver, arg_count)
      super(receiver)

      @method = method
      @receiver = receiver
      @arg_count = arg_count
    end

    attr_reader :method, :receiver, :arg_count

    def asm
      "$r#{receiver} = send #{@method}, $r#{@receiver}, #{@arg_count}"
    end

    def reads
      (@receiver..@receiver+@arg_count).to_a
    end
  end

  class Return < Operation
    include Read
    include Terminator

    def initialize(source)
      @source = source
    end

    attr_reader :source

    def asm
      "ret $r#{@source}"
    end
  end
end
