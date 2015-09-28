module Rubinius
  class Mirror
    class Thread < Mirror
      def group=(group)
        Rubinius.invoke_primitive :object_set_ivar, @object, :@group, group
      end
    end
  end
end
