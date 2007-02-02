module Spec
  module Expectations
    module Should
      class Base
        unless defined? @@differ
          @@differ = nil
        end
        def self.differ=(differ)
          @@differ = differ
        end

        alias old_default_message default_message
        def default_message(expectation, expected=:no_expectation_specified)
          result = old_default_message(expectation, expected)
          if expected != :no_expectation_specified
            if expected.is_a?(String)
              result << "\nDiff:" << @@differ.diff_as_string(@target.to_s, expected) unless @@differ.nil?
            elsif ! @target.is_a? Proc
              result << "\nDiff:" << @@differ.diff_as_object(@target, expected) unless @@differ.nil?
            end
          end

          result
        end
      end
    end
  end
end
