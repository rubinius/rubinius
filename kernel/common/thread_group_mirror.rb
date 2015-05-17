module Rubinius
  class Mirror
    class ThreadGroup < Mirror
      self.subject = ::ThreadGroup

      def remove(thread)
        ary = Rubinius.invoke_primitive :object_get_ivar, @object, :@threads
        ary.delete thread
      end

    end
  end
end
