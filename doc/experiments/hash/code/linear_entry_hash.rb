module Rubinius
  class LinearProbe < Hash
    class Entry
      attr_accessor :key
      attr_accessor :key_hash
      attr_accessor :value

      def initialize(key, key_hash, value)
        @key      = key
        @key_hash = key_hash
        @value    = value
      end

      def match?(key, key_hash)
        key.equal?(@key) or (@key_hash == key_hash and key.eql? @key)
      end
    end

    class Iterator
      def initialize(entries)
        @entries  = entries
        @capacity = entries.size
        @index    = -1
      end

      def next
        while (@index += 1) < @capacity
          if entry = @entries[@index]
            return entry
          end
        end
      end
    end

    attr_reader :size

    alias_method :length, :size

    def key_index(key_hash)
      key_hash & @mask
    end

    def find_entry(key)
      key_hash = key.hash

      index = key_index key_hash

      i = 0
      while i < MAX_PROBE
        if entry = @entries[index+i] and entry.match? key, key_hash
          return entry
        end
        i += 1
      end
    end

    def [](key)
      if entry = find_entry(key)
        entry.value
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

      i = -1
      total = entries.size
      while (i += 1) < total
        next unless entry = entries[i]
        index = key_index entry.key_hash

        j = 0
        while j < MAX_PROBE
          unless @entries[index]
            @entries[index] = entry
            break
          end

          index += 1
          j += 1
        end

        # we exceeded max_probe, start over.
        redistribute entries if j == MAX_PROBE
      end

      self
    end
    private :redistribute

    def []=(key, value)
      key_hash = key.hash

      slot = nil
      index = key_index key_hash

      i = 0
      while i < MAX_PROBE
        if entry = @entries[index+i]
          if entry.match? key, key_hash
            return entry.value = value
          end
        else
          slot = index + i unless slot
        end

        i += 1
      end

      if slot
        if key.kind_of? String
          key = key.dup
          key.freeze
        end

        @entries[slot] = Entry.new key, key_hash, value
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

      i = 0
      while i < MAX_PROBE
        if entry = @entries[index] and entry.match? key, key_hash
          @entries[index] = nil
          @size -= 1
          return entry.value
        end
        index += 1
        i += 1
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
    MAX_PROBE      = 3
    EXTRA_CAPACITY = 3

    def self.allocate
      hash = super()
      Rubinius.privately { hash.setup }
      hash
    end

    # The +capacity+ must be a power of 2!
    def setup(capacity = MIN_SIZE)
      @capacity = capacity + EXTRA_CAPACITY
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
        yield entry.key, entry.value
      end
      self
    end

    alias_method :each_pair, :each_item

    def shift
      return default(nil) if empty?

      index = 0
      while index < @capacity
        if entry = @entries[index]
          @entries[index] = nil
          @size -= 1
          return entry.key, entry.value
        end

        index += 1
      end
    end

    def fetch(key, default = Undefined)
      if entry = find_entry(key)
        return entry.value
      end

      return yield(key) if block_given?
      return default unless default.equal?(Undefined)
      raise IndexError, 'key not found'
    end

    def select
      selected = []
      i = to_iter
      while entry = i.next
        key = entry.key
        value = entry.value
        if yield(key, value)
          selected << [key, value]
        end
      end

      selected
    end
  end
end
