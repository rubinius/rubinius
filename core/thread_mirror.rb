module Rubinius
  class Mirror
    class Thread < Mirror
      StatusRun = 1
      StatusSleep = 2
      StatusDead = 3
      StatusException = 4

      def group=(group)
        Rubinius.invoke_primitive :object_set_ivar, @object, :@group, group
      end

      def thread_status
        Rubinius.invoke_primitive :thread_status, @object
      end

      def alive?
        s = thread_status
        s == StatusRun or s == StatusSleep
      end

      def stop?
        thread_status != StatusRun
      end

      def status
        case thread_status
        when StatusRun
          "run"
        when StatusSleep
          "sleep"
        when StatusDead
          false
        when StatusException
          nil
        end
      end

      def finish
        Rubinius::Mirror.reflect(@object.group).remove @object

        if exception = @object.exception
          if $DEBUG
            STDERR.puts "Exception in thread: #{exception.message} (#{exception.class})"
          end

          if @object.abort_on_exception or ::Thread.abort_on_exception
            ::Thread.main.raise exception
          end
        end
      end
    end
  end
end
