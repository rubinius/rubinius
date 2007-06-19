require 'stringio'
require 'rbconfig'

dir = File.dirname(__FILE__)
lib_path = File.expand_path("#{dir}/../lib")
$LOAD_PATH.unshift lib_path unless $LOAD_PATH.include?(lib_path)

require 'spec'
require 'spec/mocks'
require 'hpricot'
spec_classes_path = File.expand_path("#{dir}/../spec/spec/spec_classes")
require spec_classes_path unless $LOAD_PATH.include?(spec_classes_path)

module Spec
  module Matchers
    def fail
      raise_error(Spec::Expectations::ExpectationNotMetError)
    end

    def fail_with(message)
      raise_error(Spec::Expectations::ExpectationNotMetError, message)
    end

    class Pass
      def matches?(proc, &block)
        begin
          proc.call
          true
        rescue => @error
          false
        end
      end

      def failure_message
        @error.message + "\n" + @error.backtrace.join("\n")
      end
    end

    def pass
      Pass.new
    end
  end
end
