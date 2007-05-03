module Spec
  module Mocks
    module ArgumentConstraintMatchers

      # Shortcut for creating an instance of Spec::Mocks::DuckTypeArgConstraint
      def duck_type(*args)
        return Spec::Mocks::DuckTypeArgConstraint.new(*args)
      end

    end
  end
end