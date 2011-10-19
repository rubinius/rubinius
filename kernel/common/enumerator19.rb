Enumerator = Enumerable::Enumerator

module Enumerable
  class Enumerator
    def initialize(obj=undefined, iter=:each, *args)
      if obj.equal? undefined
        # TODO: implement Yielder form
      else
        @object = obj
        @iter = iter.to_sym
        @args = args
        @generator = nil
      end
    end

    def with_index(offset=0)
      if offset
        offset = Rubinius::Type.coerce_to offset, Integer, :to_int
      else
        offset = 0
      end

      return to_enum(:with_index, offset) unless block_given?

      each do |o|
        val = yield(o, offset)
        offset += 1
        val
      end
    end
  end
end
