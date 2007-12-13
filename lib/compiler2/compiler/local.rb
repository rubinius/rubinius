class Compiler
  
  class LocalScope
    def initialize(scope)
      @scope = scope
      @names = []
      @locals = Hash.new { |h,k| h[k] = Compiler::Local.new(scope, k) }
    end
    
    def [](name)
      unless @locals.key? name
        @names << name        # Maintain insertion order
      end
        
      return @locals[name]
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
  end
  
  class Local
    def initialize(scope, name)
      @scope = scope
      @name = name
      @on_stack = false
      @argument = false
      @stack_position = nil
      @slot = nil
      @on_block = false
    end
    
    attr_reader :name
    
    def inspect
      "#<#{self.class} #{@name} #{@on_stack.inspect}/#{@slot.inspect}/#{@stack_position.inspect}>"
    end
        
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
      @on_stack = !default
    end
    
    def assigned!
      if @argument
        @on_stack = false
      end
    end
  end
end
