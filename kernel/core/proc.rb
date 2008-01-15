# depends on: class.rb

class Proc
  self.instance_fields = 3
  ivar_as_index :__ivars__ => 0, :block => 1, :check_args => 2

  def block; @block ; end

  def block=(other)
    @block = other
  end

  def check_args=(other)
    @check_args = other
  end

  def self.from_environment(env, check_args=false)
    if env.nil?
      nil
    elsif env.kind_of?(BlockEnvironment)
      obj = allocate()
      obj.block = env
      obj.check_args = check_args
      obj
    elsif env.respond_to? :to_proc
      env.to_proc
    else
      raise ArgumentError.new("Unable to turn a #{env.inspect} into a Proc")
    end
  end

  def self.new(&block)
    if block
      return block
    else
      # This behavior is stupid.
      be = MethodContext.current.sender.block
      if be
        return from_environment(be)
      else
        raise ArgumentError, "tried to create a Proc object without a block"
      end
    end
  end

  # Return the proc given to the currently running method or
  # to the given MethodContext/Binding.
  #
  #   def bar(&prc)
  #      a = [prc.nil?, Proc.given.nil?]
  #      a << block_given? == !Proc.given.nil?
  #      if block_given?
  #         a << prc.object_id == Proc.given.object_id
  #         a << prc.block.object_id == Proc.given.block.object_id
  #         a << Proc.given.call(21)
  #      end
  #      a
  #   end
  #
  #   bar()                 # => [true, true, true]
  #   bar() { |n| n * 2 }   # => [false, false, true, false, true, 42]
  #
  # An example mind trick using MethodContext.
  #
  #   def stormtrooper
  #      yield "Let me see your identification."
  #      obiwan { |reply| puts "Obi-Wan: #{reply}" }
  #   end
  #
  #   def obiwan
  #      yield "[with a small wave of his hand] You don't need to see his identification."
  #      ctx = MethodContext.current.sender
  #      Proc.given(ctx).call("We don't need to see his identification.")
  #   end
  #
  #   stormtrooper { |msg| puts "Stormtrooper: #{msg}" }
  #
  # produces the following output:
  #
  #   Stormtrooper: Let me see your identification.
  #   Obi-Wan: [with a small wave of his hand] You don't need to see his identification.
  #   Stormtrooper: We don't need to see his identification
  #
  # Using a binging to obtain the given proc where the binding was created
  #
  #   def stormtrooper
  #      binding
  #   end
  #
  #   def obiwan(trick)
  #      yield "These aren't the droids you're looking for."
  #      trick.call("There aren't the droids we're looking for.")
  #      yield "He can go about his business."
  #      trick.call("You can go about your business.")
  #   end
  #
  #   trick = stormtrooper { |msg| puts "Stormtrooper: #{msg}" }
  #   obiwan(Proc.given(trick)) { |msg| puts "Obi-Wan: #{msg}" }
  #
  def self.given(ctx = nil)
    case ctx
    when nil
      ctx = MethodContext.current.sender.block
    when MethodContext
      ctx = ctx.block
      # when BlockEnvironment
      # when Binding
      # ctx = ctx.context
    end
    from_environment(ctx)
  end

  def inspect
    "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{self.block.file}:#{self.block.line}>"
  end

  def arity
    @block.arity
  end

  def to_proc
    self
  end

  def call(*args)
    @block.call(*args)
  end

  alias_method :[], :call

  class Function < Proc
    ivar_as_index :block => 1

    def call(*args)
      a = arity()
      unless a < 0 or a == 1 or args.size == a
        raise ArgumentError, "wrong number of arguments (#{args.size} for #{arity})"
      end

      begin
        @block.call(*args)
      rescue IllegalLongReturn => e
        return e.return_value
      end
    end
    alias_method :[], :call
  end
end
