module Spec
  module Runner
    class ExecutionContext
      module InstanceMethods
        def initialize(spec)
          @spec = spec
        end

        def mock(name, options={})
          Spec::Mocks::Mock.new(name, options)
        end

        def duck_type(*args)
          return Spec::Mocks::DuckTypeArgConstraint.new(*args)
        end

        def violated(message="")
          raise Spec::Expectations::ExpectationNotMetError.new(message)
        end

      end
      include InstanceMethods
    end
  end
end