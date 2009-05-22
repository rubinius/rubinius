module Rubinius
  class Cuckoo < Hash
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
      def initialize(maximum, entries)
        @maximum = maximum
        @entries  = entries
        @index    = -1
      end

      # Returns the next object or +nil+.
      def next
        while (@index += 1) < @maximum
          entry = @entries[@index]
          return entry if entry
        end
      end
    end

    attr_reader :size

    alias_method :length, :size

    def key_index1(key_hash)
      key_hash & @mask
    end
    private :key_index1

    def key_index2(key_hash)
      (key_hash.hash & @mask) + @capacity
    end
    private :key_index2

    def find_entry(key)
      key_hash = key.hash
      entry = @entries[key_index1(key_hash)]
      return entry if entry and entry.match? key, key_hash

      entry = @entries[key_index2(key_hash)]
      return entry if entry and entry.match? key, key_hash
    end

    def [](key)
      if entry = find_entry(key)
        entry.value
      else
        default key
      end
    end

    def rehash
      i = to_iter

      # TODO: grow smaller too
      setup @maximum
      @max_loop += MAX_LOOP_INCR

      while entry = i.next
        insert entry.key, entry.value, false
      end

      self
    end

    def insert(key, value, updating)
      rehash unless @size < @capacity

      key_hash = key.hash
      index = key_index1 key_hash
      unless entry = @entries[index]
        @entries[index] = Entry.new key, key_hash, value
        @size += 1
        return
      end

      if updating
        if entry.match? key, key_hash
          entry.value = value
          return
        end

        index2 = key_index2 key_hash
        if entry2 = @entries[index2] and entry2.match? key, key_hash
          entry2.value = value
          return
        end
      end

      new_entry = Entry.new key, key_hash, value

      i = 1
      while i < @max_loop
        # Boot the found entry from the nest and take it over.
        @entries[index] = new_entry
        new_entry = entry

        # Repeat with the booted entry as the new_entry
        index = key_index2 new_entry.key_hash
        unless entry = @entries[index]
          @entries[index] = new_entry
          @size += 1
          return
        end

        @entries[index] = new_entry
        new_entry = entry

        # If there is no entry for this key, set the entry.
        index = key_index1 new_entry.key_hash
        unless entry = @entries[index]
          @entries[index] = new_entry
          @size += 1
          return
        end

        i += 1
      end

      rehash

      insert new_entry.key, new_entry.value, false
    end

    def []=(key, value)
      if key.kind_of? String
        key = key.dup
        key.freeze
      end
      insert key, value, true

      value
    end

    alias_method :store, :[]=

    def delete(key)
      key_hash = key.hash

      index = key_index1 key_hash
      if entry = @entries[index] and entry.match? key, key_hash
        @entries[index] = nil
        @size -= 1
        return entry.value
      end

      index = key_index2 key_hash
      if entry = @entries[index] and entry.match? key, key_hash
        @entries[index] = nil
        @size -= 1
        return entry.value
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

    # The Cuckoo algorthim calls for forcing a rehash after max_loop
    # iterations when inserting keys. The calculation for max_loop is:
    #
    #   r = (1 * epsilon) * n
    #   max_loop = (3 * log(r) / log(1 + epsilon)).ceil
    #
    # where epsilon > 0 and n is our +capacity+.
    #
    # The following base and increment are based on epsilon = 0.5 and
    # a value of n = 8.
    MAX_LOOP_BASE = 19
    MAX_LOOP_INCR = 5
    MIN_SIZE      = 8

    def self.allocate
      hash = super()
      Rubinius.privately { hash.setup }
      hash
    end

    # The +capacity+ must be a power of 2! If you call #setup with a capacity
    # other than MIN_SIZE, you must set @max_loop correctly.
    def setup(capacity = MIN_SIZE)
      @capacity = capacity
      @maximum = capacity * 2
      @mask = capacity - 1
      @size = 0
      @max_loop = MAX_LOOP_BASE if capacity == MIN_SIZE
      @entries = Rubinius::Tuple.new @maximum
    end
    private :setup

    def clear
      setup
      self
    end

    def to_iter
      Iterator.new @maximum, @entries
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

      i = 0
      while i < @maximum
        if entry = @entries[i]
          @entries[i] = nil
          @size -= 1
          return entry.key, entry.value
        end

        i += 1
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
        if yield(entry.key, entry.value)
          selected << [entry.key, entry.value]
        end
      end

      selected
    end
  end
end
