class Compiler

  ##
  # The precursor to a CompiledMethod

  # TODO: This is temporary while transitioning to CompilerNG.
  # Remove when the transition is completed.

  class MethodDescription
    def initialize(gen_class, locals)
      @generator = gen_class.new
      @locals = locals
      @required = 0
      @optional = 0
      @splat = nil
      @for_block = false
      @name = :__unknown__
    end

    attr_reader :generator, :locals
    attr_accessor :required, :optional, :name, :args, :splat, :for_block

    def run(container, body)
      @generator.run(body)
      @required, @optional, @splat = container.argument_info
      @name = container.name if container.name
    end

    def ==(desc)
      desc.kind_of? MethodDescription and @generator == desc.generator
    end

    def to_cmethod
      @generator.to_cmethod(self)
    end

    def to_ary
      [:method_description, @generator]
    end

    def pretty_inspect
      to_ary.pretty_inspect
    end

    def inspect
      to_ary.inspect
    end
  end
end
