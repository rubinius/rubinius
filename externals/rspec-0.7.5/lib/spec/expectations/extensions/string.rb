module Spec
  module Expectations
    # rspec adds all of these expectations to every String object.
    module StringExpectations

      # Passes if receiver =~ expression
      def should_match(expression)
        should.match(expression)
      end

      # Passes unless receiver =~ expression
      def should_not_match(expression)
        should.not.match(expression)
      end

    end
  end
end

class String
  include Spec::Expectations::StringExpectations
end