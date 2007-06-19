module Spec
  module Runner
    module Formatter
      # Baseclass for formatters that implements all required methods as no-ops. 
      class BaseFormatter
        def initialize(where)
          @where = where
        end
        
        # This method is invoked before any examples are run, right after
        # they have all been collected. This can be useful for special
        # formatters that need to provide progress on feedback (graphical ones)
        #
        # This method will only be invoked once, and the next one to be invoked
        # is #add_behaviour
        def start(example_count)
        end

        # This method is invoked at the beginning of the execution of each behaviour.
        # +name+ is the name of the behaviour and +first+ is true if it is the
        # first behaviour - otherwise it's false.
        #
        # The next method to be invoked after this is #example_failed or #example_finished
        def add_behaviour(name)
        end

        # This method is invoked when an example starts. +name+ is the name of the
        # example.
        def example_started(name)
        end

        # This method is invoked when an example passes. +name+ is the name of the
        # example.
        def example_passed(name)
        end

        # This method is invoked when an example fails, i.e. an exception occurred
        # inside it (such as a failed should or other exception). +name+ is the name
        # of the example. +counter+ is the sequence number of the failure
        # (starting at 1) and +failure+ is the associated Failure object.
        def example_failed(name, counter, failure)
        end
        
        # This method is invoked when an example is not yet implemented (i.e. has not been provided a block). 
        # +name+ is the name of the example.
        def example_not_implemented(name)
        end

        # This method is invoked after all of the examples have executed. The next method
        # to be invoked after this one is #dump_failure (once for each failed example),
        def start_dump
        end

        # Dumps detailed information about an example failure.
        # This method is invoked for each failed example after all examples have run. +counter+ is the sequence number
        # of the associated example. +failure+ is a Failure object, which contains detailed
        # information about the failure.
        def dump_failure(counter, failure)
        end
      
        # This method is invoked at the very end.
        def dump_summary(duration, example_count, failure_count, not_implemented_count)
        end

      end
    end
  end
end
