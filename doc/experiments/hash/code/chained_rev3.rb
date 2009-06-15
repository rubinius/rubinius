module Rubinius
  class Chained < Hash
    class Entry
      attr_accessor :key
      attr_accessor :key_hash
      attr_accessor :value
      attr_accessor :next

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
      attr_reader :index

      def initialize(entries, capacity)
        @entries  = entries
        @capacity = capacity
        @index    = -1
      end

      # Returns the next object or +nil+.
      def next(entry)
        if entry and entry = entry.next
          return entry
        end

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

      entry = @entries[key_index(key_hash)]
      while entry
        if entry.match? key, key_hash
          return entry
        end
        entry = entry.next
      end
    end
    private :find_entry

    def [](key)
      if entry = find_entry(key)
        entry.value
      else
        default key
      end
    end

    def rehash
      redistribute @entries, true
    end

    def redistribute(entries, rehash=false)
      capacity = @capacity

      # TODO: grow smaller too
      setup @capacity * 2, @max * 2, @size

      i = -1
      while (i += 1) < capacity
        next unless old = entries[i]
        while old
          old.next = nil if nxt = old.next

          if rehash
            key_hash = old.key_hash = old.key.hash
          else
            key_hash = old.key_hash
          end

          index = key_index key_hash
          unless entry = @entries[index]
            @entries[index] = old
          else
            last = entry
            while entry = entry.next
              last = entry
            end
            last.next = old
          end

          old = nxt
        end
      end

      self
    end

    def new_entry(key, key_hash, value)
      if key.kind_of? String
        key = key.dup
        key.freeze
      end

      @size += 1
      Entry.new key, key_hash, value
    end
    private :new_entry

    def []=(key, value)
      redistribute @entries if @size > @max

      key_hash = key.hash
      index = key_index key_hash

      unless entry = @entries[index]
        @entries[index] = new_entry key, key_hash, value
        return value
      else
        if entry.match? key, key_hash
          return entry.value = value
        end
      end

      last = entry
      entry = entry.next
      while entry
        if entry.match? key, key_hash
          return entry.value = value
        end

        last = entry
        entry = entry.next
      end
      last.next = new_entry key, key_hash, value

      value
    end

    alias_method :store, :[]=

    def delete(key)
      key_hash = key.hash

      index = key_index key_hash
      if entry = @entries[index]
        if entry.match? key, key_hash
          @entries[index] = entry.next
          @size -= 1
          return entry.value
        end

        last = entry
        while entry = entry.next
          if entry.match? key, key_hash
            last.next = entry.next
            @size -= 1
            return entry.value
          end
          last = entry
        end
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

    MIN_SIZE  = 16
    MAX_ENTRIES = 12
    MAX_CHAIN = 3

    def self.allocate
      hash = super()
      Rubinius.privately { hash.setup }
      hash
    end

    # The +capacity+ must be a power of 2!
    def setup(capacity=MIN_SIZE, max=MAX_ENTRIES, size=0)
      @capacity = capacity
      @mask     = capacity - 1
      @max      = max
      @size     = size
      @entries  = Rubinius::Tuple.new capacity
    end
    private :setup

    def clear
      setup
      self
    end

    def to_iter
      Iterator.new @entries, @capacity
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
      entry = nil
      while entry = i.next(entry)
        yield entry.key, entry.value
      end
      self
    end

    alias_method :each_pair, :each_item

    def shift
      return default(nil) if empty?

      i = to_iter
      entry = nil
      if entry = i.next(entry)
        @entries[i.index] = entry.next
        @size -= 1
        return entry.key, entry.value
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
      entry = nil
      while entry = i.next(entry)
        if yield(entry.key, entry.value)
          selected << [entry.key, entry.value]
        end
      end

      selected
    end

    #-----------------------------

    def each
      i = to_iter
      entry = nil
      while entry = i.next(entry)
        yield [entry.key, entry.value]
      end
      self
    end

    def each_pair
      i = to_iter
      entry = nil
      while entry = i.next(entry)
        yield entry.key, entry.value
      end
      self
    end
  end
end
