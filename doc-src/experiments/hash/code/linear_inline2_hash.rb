module Rubinius
  class LinearProbe < Hash
    class Iterator
      def initialize(entries)
        @entries  = entries
        @capacity = entries.size
        @index    = -3
      end

      def next
        while (@index += 3) < @capacity
          if @entries[@index]
            return @entries[@index+1], @entries[@index+2]
          end
        end
      end
    end

    attr_reader :size

    alias_method :length, :size

    def key_index(key_hash)
      (key_hash & @mask) * 3
    end

    def match?(index, key, key_hash)
      return unless kh = @entries[index]
      k = @entries[index+1]
      key.equal?(k) or (kh == key_hash and key.eql? k)
    end

    def find_entry(key)
      key_hash = key.hash

      index = key_index key_hash
      if match? index, key, key_hash
        return @entries[index+1], @entries[index+2]
      end

      index += 3
      if match? index, key, key_hash
        return @entries[index+1], @entries[index+2]
      end
    end

    def [](key)
      if entry = find_entry(key)
        entry[1]
      else
        default key
      end
    end

    def rehash
      redistribute @entries
    end

    def redistribute(entries)
      # TODO: grow smaller too
      setup((@mask + 1) * 2)

      i = -3
      total = entries.size
      while (i += 3) < total
        next unless key_hash = entries[i]
        index = key_index key_hash

        unless @entries[index]
          @entries.copy_from entries, i, 3, index
          next
        end

        index += 3
        unless @entries[index]
          @entries.copy_from entries, i, 3, index
          next
        end

        # we exceeded max_probe, start over.
        redistribute entries
      end

      self
    end
    private :redistribute

    def []=(key, value)
      key_hash = key.hash

      slot = nil
      index = key_index key_hash

      if @entries[index]
        if match? index, key, key_hash
          return @entries[index+2] = value
        end
      else
        slot = index
      end

      index += 3
      if @entries[index]
        if match? index, key, key_hash
          return @entries[index+2] = value
        end
      else
        slot = index unless slot
      end

      if slot
        if key.kind_of? String
          key = key.dup
          key.freeze
        end

        @entries[slot]   = key_hash
        @entries[slot+1] = key
        @entries[slot+2] = value
        @size += 1
      else
        redistribute @entries
        self[key] = value
      end

      value
    end

    alias_method :store, :[]=

    def delete(key)
      key_hash = key.hash
      index = key_index key_hash

      if match? index, key, key_hash
        value = @entries[index+2]
        @entries[index] = nil
        @size -= 1
        return value
      end

      index += 3
      if match? index, key, key_hash
        value = @entries[index+2]
        @entries[index] = nil
        @size -= 1
        return value
      end

      return yield(key) if block_given?
    end

    #-------------------------------------------------------------------------

    # TODO: rework when not a subclass of Hash
    def self.[](*args)
      if args.size == 1
        obj = args.first
        if obj.kind_of? Hash
          hsh = new
          obj.each do |key, value|
            hsh[key] = value
          end
          return hsh
        elsif obj.respond_to? :to_hash
          return new.replace(Type.coerce_to(obj, Hash, :to_hash))
        end
      end

      if args.size & 1 == 1
        raise ArgumentError, "Expected an even number, got #{args.length}"
      end

      hsh = new
      i = 0
      while i < args.size
        hsh[args[i]] = args[i+1]
        i += 2
      end
      hsh
    end

    MIN_SIZE       = 16
    EXTRA_CAPACITY = 3

    def self.allocate
      hash = super()
      Rubinius.privately { hash.setup }
      hash
    end

    # The +capacity+ must be a power of 2!
    def setup(capacity = MIN_SIZE)
      @capacity = capacity * 3 + EXTRA_CAPACITY
      @mask = capacity - 1
      @size = 0
      @entries = Rubinius::Tuple.new @capacity
    end
    private :setup

    def clear
      setup
      self
    end

    def to_iter
      Iterator.new @entries
    end

    def key?(key)
      return true if find_entry key

      false
    end

    alias_method :has_key?, :key?
    alias_method :include?, :key?
    alias_method :member?, :key?

    def each_item
      i = to_iter
      while entry = i.next
        yield entry[0], entry[1]
      end
      self
    end

    alias_method :each_pair, :each_item

    def shift
      return default(nil) if empty?

      index = 0
      while index < @capacity
        if @entries[index]
          @entries[index] = nil
          @size -= 1
          return @entries[index+1], @entries[index+2]
        end

        index += 3
      end
    end

    def fetch(key, default = Undefined)
      if entry = find_entry(key)
        return entry[1]
      end

      return yield(key) if block_given?
      return default unless default.equal?(Undefined)
      raise IndexError, 'key not found'
    end

    def select
      selected = []
      i = to_iter
      while entry = i.next
        key = entry[0]
        value = entry[1]
        if yield(key, value)
          selected << [key, value]
        end
      end

      selected
    end
  end
end
