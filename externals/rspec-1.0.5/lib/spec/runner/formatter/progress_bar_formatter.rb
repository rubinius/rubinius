module Spec
  module Runner
    module Formatter
      class ProgressBarFormatter < BaseTextFormatter
        def add_behaviour(name)
        end
      
        def example_failed(name, counter, failure)
          @output.print failure.expectation_not_met? ? red('F') : magenta('F')
          @output.flush
        end

        def example_passed(name)
          @output.print green('.')
          @output.flush
        end
      
        def example_not_implemented(name)
          @output.print yellow('*')
          @output.flush
        end
        
        def start_dump
          @output.puts
          @output.flush
        end
      end
    end
  end
end
