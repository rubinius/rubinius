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

      # Implementation based on the binomial_coefficient
      # implemented on MRI array.c.
      def binomial_coefficient(comb, size)
        comb = size-comb if (comb > size-comb)
        return 0 if comb < 0
        descending_factorial(size, comb) / descending_factorial(comb, comb)
      end

      # Implementation based on the rb_ary_combination_size
      # implemented on MRI array.c.
      def combination_size(num)
        binomial_coefficient(num, @object.size)
      end

      # Implementation based on the descending_factorial
      # implemented on MRI array.c.
      # Comment from there:
      # Returns the product of from, from-1, ..., from - how_many + 1.
      # http://en.wikipedia.org/wiki/Pochhammer_symbol
      def descending_factorial(from, how_many)
        cnt = how_many >= 0 ? 1 : 0
        while (how_many) > 0
          cnt = cnt*(from)
          from -= 1
          how_many -= 1
        end
        cnt
      end

      # Implementation based on the rb_ary_permutation_size
      # implemented on MRI array.c.
      def permutation_size(num)
        n = @object.size
        if undefined.equal? num
          k = @object.size
        else
          k = Rubinius::Type.coerce_to_collection_index num
        end
        descending_factorial(n, k)
      end

      def repeated_combination_size(combination_size)
        return 1 if combination_size == 0
        return binomial_coefficient(combination_size, @object.size + combination_size - 1)
      end

      def repeated_permutation_size(combination_size)
        return 0 if combination_size < 0
        @object.size ** combination_size
      end
    end
  end
end
