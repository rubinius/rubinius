class Compiler

  ##
  # Tracks local variables in a MethodDescription

  class LocalScope
    def initialize(scope)
      @scope = scope
      @names = []
      @locals = Hash.new { |h,k| h[k] = Local.new(@scope, k) }
      @from_eval = false
      @skip = false
    end

    attr_accessor :from_eval

    def [](name)
      unless name.kind_of? Symbol
        raise ArgumentError, "must be a symbol: #{name.inspect}"
      end

      unless @locals.key? name
        @names << name        # Maintain insertion order
      end

      return @locals[name]
    end

    def keys
      @locals.keys
    end

    def key?(name)
      @locals.key? name
    end

    def formalize!
      @names.each do |var|
        @locals[var].formalize!
      end
    end

    def size
      @names.size
    end

    def encoded_order
       # figure out the size
       size = 0
       @names.each do |name|
         size += 1 unless @locals[name].on_stack?
       end

       return nil if size == 0

       tup = Rubinius::Tuple.new(size)

       @names.each do |name|
         var = @locals[name]
         unless var.on_stack?
           tup[var.slot] = name
         end
       end

       return tup
    end

    def skip?
      @skip
    end

    def skip!
      @skip = true
    end

    attr_reader :locals
    attr_accessor :scope
  end

  ##
  # A local variable in a LocalScope

  class Local
    def initialize(scope, name)
      @scope = scope
      @name = name
      @on_stack = false
      @argument = false
      @stack_position = nil
      @slot = nil
      @on_block = false
      @position = nil
    end

    attr_reader :name

    def inspect
      "#<#{self.class} #{@name} @slot=#{@slot.inspect}, @on_block=#{@on_block.inspect}>"
    end

    attr_reader :name

    def formalize!
      if @argument
        return if @on_stack
      elsif @on_stack
        @stack_position = @scope.allocate_stack
        # The scope returns nil if this local should not be
        # on the stack.
        return if @stack_position
        @on_stack = false
      end

      @slot = @scope.allocate_slot unless @slot

      # locals in blocks get their slot set when the var is created, not
      # later on when formalized
    end

    def slot
      if @slot.nil?
        raise Error, "Attempted to use a unformalized local: #{@name}"
      end

      return @slot
    end

    attr_writer :slot

    def stack_position
      if @argument
        @position
      else
        @stack_position
      end
    end

    def in_locals!
      @on_stack = false
    end

    def created_in_block!(slot)
      @slot = slot
      @on_block = true
    end

    def created_in_block?
      @on_block
    end

    def access_in_block!
      @on_stack = false
    end

    def argument?
      @argument
    end

    def on_stack?
      @on_stack
    end

    def argument!(position, default=false)
      @position = position
      @argument = true
    end

    def assigned!
      if @argument
        @on_stack = false
      end
    end
  end
end
