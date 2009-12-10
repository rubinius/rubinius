##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

module Rubinius
  class BlockEnvironment
    attr_accessor :scope
    attr_accessor :top_scope
    attr_accessor :local_count
    attr_accessor :method # The CompiledMethod object that we were called from

    attr_accessor :proc_environment
    attr_accessor :metadata_container

    def from_proc?
      @proc_environment
    end

    def under_context(scope, cmethod)
      @top_scope = scope
      @scope = scope
      @method = cmethod
      @local_count = cmethod.local_count

      return self
    end

    def receiver=(obj)
      @top_scope.self = obj
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
      call_under obj, method.scope, *args
    end

    def arity
      if method.splat >= 0 or method.splat == -2
        -(method.required_args + 1)
      else
        method.required_args
      end
    end

    # TODO: are file,line actually used?
    def file
      method.file
    end

    def line
      method.line_from_ip(0)
    end
  end
end
