module Rubinius
  class Mirror
    class Array < Mirror
      self.subject = ::Array

      def self.reflect(object)
        m = super(object)

        if Rubinius::Type.object_kind_of? m.object, ::Array
          m
        elsif ary = Rubinius::Type.try_convert(m.object, ::Array, :to_ary)
          super(ary)
        else
          message = "expected Array, given #{Rubinius::Type.object_class(object)}"
          raise TypeError, message
        end
      end

      # TODO: implement mirror_attribute :name
      def total
        Rubinius.invoke_primitive :object_get_ivar, @object, :@total
      end

      def total=(value)
        Rubinius.invoke_primitive :object_set_ivar, @object, :@total, value
      end

      def tuple
        Rubinius.invoke_primitive :object_get_ivar, @object, :@tuple
      end

      def tuple=(value)
        Rubinius.invoke_primitive :object_set_ivar, @object, :@tuple, value
      end

      def start
        Rubinius.invoke_primitive :object_get_ivar, @object, :@start
      end

      def start=(value)
        Rubinius.invoke_primitive :object_set_ivar, @object, :@start, value
      end
    end
  end
end
