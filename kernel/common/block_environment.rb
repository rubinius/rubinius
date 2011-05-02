##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

module Rubinius
  class BlockEnvironment
    attr_reader :scope
    attr_reader :top_scope

    # The CompiledMethod that implements the code for the block
    attr_reader :code

    attr_accessor :proc_environment
    attr_reader :metadata_container

    def from_proc?
      @proc_environment
    end

    def under_context(scope, cmethod)
      @top_scope = scope
      @scope = scope
      @code = cmethod
      @module = cmethod.scope.module

      return self
    end

    def change_name(name)
      @code = @code.change_name name
    end

    def repoint_scope(where)
      @code.scope.using_current_as where
    end

    def disable_scope!
      @code.scope.using_disabled_scope
    end

    def static_scope
      @code.scope
    end

    def to_binding
      Binding.setup @scope, @code, @code.scope
    end

    def set_eval_binding(bind)
      @code.scope.script.eval_binding = bind
    end

    ##
    # First field of the tuple in @metadata_container holds a boolean
    # indicating if the context is from eval

    def from_eval?
      @metadata_container and @metadata_container[0]
    end

    def from_eval!
      @metadata_container = Tuple.new(1) unless @metadata_container
      @metadata_container[0] = true
    end

    def make_independent
      @top_scope = @top_scope.dup
      @scope = @scope.dup
    end

    def call_on_instance(obj, *args)
      call_under obj, @code.scope, *args
    end

    def arity
      if @code.splat and (@code.splat >= 0 or @code.splat == -2)
        -(@code.required_args + 1)
      else
        @code.required_args
      end
    end

    def file
      @code.file
    end

    def line
      @code.line_from_ip(0)
    end
  end
end
