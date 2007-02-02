require 'spec/expectations/sugar'
require 'spec/expectations/errors'
require 'spec/expectations/extensions'
require 'spec/expectations/should'
require 'spec/expectations/message_builder'

module Spec
  # See Spec::Expectations::ObjectExpectations for expectations that
  # are available on all objects.
  #
  # See Spec::Expectations::ProcExpectations for expectations that
  # are available on Proc objects.
  #
  # See Spec::Expectations::NumericExpectations for expectations that
  # are available on Numeric objects.
  module Expectations
    class << self
      # raises a Spec::Expectations::ExpecationNotMetError with message
      def fail_with(message)
        Kernel::raise(Spec::Expectations::ExpectationNotMetError.new(message))
      end
      
      def build_message(actual, expectation, expected)
        message_builder.build_message(actual, expectation, expected)
      end
      
      def message_builder #:nodoc:
        @message_builder ||= MessageBuilder.new
      end
    end
  end
end