# -*- encoding: us-ascii -*-

##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

module Rubinius
  class BlockEnvironment
    attr_reader :scope
    attr_reader :top_scope
    attr_reader :module

    # The CompiledCode that implements the code for the block
    attr_reader :compiled_code

    attr_accessor :proc_environment
    attr_reader :metadata_container

    def from_proc?
      @proc_environment
    end

    def under_context(scope, code)
      @top_scope = scope
      @scope = scope
      @compiled_code = code
      @module = code.scope.module

      return self
    end

    def change_name(name)
      @compiled_code = @compiled_code.change_name name
    end

    def repoint_scope(where)
      @compiled_code.scope.using_current_as where
    end

    def disable_scope!
      @compiled_code.scope.using_disabled_scope
    end

    def constant_scope
      @compiled_code.scope
    end

    def to_binding
      Binding.setup @scope, @compiled_code, @compiled_code.scope
    end

    def set_eval_binding(bind)
      @compiled_code.scope.script.eval_binding = bind
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
      call_under obj, @compiled_code.scope, *args
    end

    def arity
      if @compiled_code.splat and
         (@compiled_code.splat >= 0 or @compiled_code.splat == -2)
        -(@compiled_code.required_args + 1)
      else
        @compiled_code.required_args
      end
    end

    def file
      @compiled_code.file
    end

    def line
      @compiled_code.defined_line
    end

    def source_location
      [file.to_s, line]
    end

      def ==(other)
        return false unless other.kind_of? BlockEnvironment

        @top_scope       == other.top_scope and
          @scope         == other.scope and
          @module        == other.module and
          @compiled_code.equivalent_body?(other.compiled_code)
      end

    class AsMethod < Executable
      attr_reader :block_env

      def ==(other)
        # Given the following code:
        # class Foo
        #   p = Proc.new { :cool }
        #   define_method :a, p
        #   define_method :a, p
        # end
        # foo = Foo.new
        # foo.method(:a)
        # foo.method(:b)
        #
        # The Method instances for :a and :b have different
        # BlockEnvironments as define_method dups the BE
        # when given a Proc.
        #
        # The methods are equal if the BEs are equal.

        @block_env == other.block_env
      end
    end
  end
end
