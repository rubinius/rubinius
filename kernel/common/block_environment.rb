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
    attr_reader :constant_scope

    attr_accessor :proc_environment

    def from_proc?
      @proc_environment
    end

    def under_context(scope, code)
      @top_scope = scope
      @scope = scope
      @compiled_code = code
      @constant_scope = code.scope
      @module = code.scope.module

      return self
    end

    def change_name(name)
      @compiled_code = @compiled_code.change_name name
    end

    def repoint_scope(where)
      @constant_scope.using_current_as where
    end

    def disable_scope!
      @constant_scope.using_disabled_scope
    end

    def to_binding
      Binding.setup @scope, @compiled_code, @constant_scope
    end

    def make_independent
      @top_scope = @top_scope.dup
      @scope = @scope.dup
    end

    def call_on_instance(obj, *args)
      call_under obj, @constant_scope, false, *args
    end

    def arity
      @compiled_code.arity
    end

    def parameters
      @compiled_code.parameters
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

    def inspect
      "#<#{self.class.name}:0x#{self.object_id.to_s(16)} scope=#{@scope.inspect} top_scope=#{@top_scope.inspect} module=#{@module.inspect} compiled_code=#{@compiled_code.inspect} constant_scope=#{@constant_scope.inspect}>"
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
        return false unless other.kind_of? AsMethod

        @block_env == other.block_env
      end

      def parameters
        @block_env.parameters
      end
    end
  end
end
