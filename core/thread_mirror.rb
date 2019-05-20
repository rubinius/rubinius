module Rubinius
  class Mirror
    class Thread < Mirror
      def group=(group)
        Rubinius.invoke_primitive :object_set_ivar, @object, :@group, group
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
