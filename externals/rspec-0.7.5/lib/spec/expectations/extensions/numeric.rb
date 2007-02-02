module Spec
  module Expectations
    module NumericExpectations
      # Passes if receiver is less than +-delta away from other
      def should_be_close(other, delta)
        should.be._close_for_rspec(other, delta)
      end

      private
      def _close_for_rspec?(other, delta)
        (self - other).abs < delta
      end
    end
  end
end

class Numeric
  include Spec::Expectations::NumericExpectations
end