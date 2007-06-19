module Spec
  module Runner
    module Formatter
      class FailingExamplesFormatter < BaseTextFormatter      
        def add_behaviour(behaviour_name)
          @behaviour_name = behaviour_name
        end
      
        def example_failed(name, counter, failure)
          @output.puts "#{@behaviour_name} #{name}"
          @output.flush
        end

        def dump_failure(counter, failure)
        end

        def dump_summary(duration, example_count, failure_count, not_implemented_count)
        end
      end
    end
  end
end
