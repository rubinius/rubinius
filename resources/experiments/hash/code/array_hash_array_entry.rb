module Rubinius
  class ArrayHash < Hash
    class Entry
      attr_reader :key, :key_hash, :value

      def initialize(tuple, index)
        @tuple    = tuple
        @index    = index
        @key_hash = @tuple[index]
        @key      = @tuple[index+1]
        @value    = @tuple[index+2]
      end

      def value=(value)
        @tuple[@index+2] = value
      end
    end

    class EntryArray
      MIN_SIZE = 9
      MAX_SIZE = 18

      attr_reader :tuple

      def initialize(key, key_hash, value)
        @tuple    = Rubinius::Tuple.new MIN_SIZE
        @capacity = MIN_SIZE
        @size     = 0
        insert 0, key, key_hash, value
      end

      def empty?
        @size == 0
      end

      def match?(index, key, key_hash)
        return unless kh = @tuple[index]
        k = @tuple[index+1]
        key.equal?(k) or (kh == key_hash and key.eql? k)
      end

      def first
        i = 0
        while i < @capacity
          return Entry.new(@tuple, i) if @tuple[i]
          i += 3
        end
      end

      # Returns the entry if found, otherwise returns nil.
      def find(key, key_hash)
        i = 0
        while i < @capacity
          return Entry.new(@tuple, i) if match? i, key, key_hash
          i += 3
        end
      end

      # Deletes and returns the entry matching key, key_hash.
      # Otherwise, returns nil.
      def delete(key, key_hash)
        i = 0
        while i < @capacity
          if match? i, key, key_hash
            entry = Entry.new @tuple, i

            @size -= 1
            @tuple[i] = nil

            return entry
          end

          i += 3
        end
      end

      # Deletes and returns the first entry encountered.
      def shift
        i = 0
        while i < @capacity
          if @tuple[i]
            entry = Entry.new @tuple, i

            @size -= 1
            @tuple[i] = nil

            return entry
          end

          i += 3
        end
      end

      def insert(index, key, key_hash, value)
        if key.kind_of? String
          key = key.dup
          key.freeze
        end

        unless index < @capacity
          @capacity *= 2
          return if @capacity > MAX_SIZE

          tuple = Rubinius::Tuple.new @capacity
          tuple.copy_from @tuple, 0, @tuple.size, 0
          @tuple = tuple
        end

        @size += 1
        @tuple[index]   = key_hash
        @tuple[index+1] = key
        @tuple[index+2] = value
      end
      private :insert

      # Returns true if the entry was inserted, false if an
      # existing entry was updated.
      def update_or_insert(key, key_hash, value)
        key_hash = key.hash
        last = nil

        i = 0
        while i < @capacity
          last = i unless @tuple[i]
          if match? i, key, key_hash
            @tuple[i + 2] = value
            return false
          end

          i += 3
        end

        index = last ? last : @capacity
        insert index, key, key_hash, value
      end
    end

    class Iterator
      def initialize(entries)
        @entries  = entries
        @capacity = entries.size
        @tuple    = nil
        @index    = -1
      end

      # Returns the next object or +nil+.
      def next
        if @tuple
          while (@tuple_index += 3) < @tuple_capacity
            return Entry.new(@tuple, @tuple_index) if @tuple[@tuple_index]
          end
        end

        while (@index += 1) < @capacity
          if array = @entries[@index]
            @tuple = array.tuple
            @tuple_index = -3
            @tuple_capacity = @tuple.size
            return self.next
          end
        end

        @tuple = nil
      end
    end

    attr_reader :size

    alias_method :length, :size

    def key_index(key_hash)
      key_hash & @mask
    end

    def find_entry(key)
      key_hash = key.hash

      array = @entries[key_index(key_hash)]
      if array and entry = array.find(key, key_hash)
        return entry
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
      i = to_iter

      # TODO: grow smaller too
      setup @capacity * 2

      while entry = i.next
        self[entry.key] = entry.value
      end

      self
    end

    def []=(key, value)
      key_hash = key.hash

      index = key_index key_hash
      unless array = @entries[index]
        @entries[index] = EntryArray.new key, key_hash, value
        @size += 1
        return value
      end

      result = array.update_or_insert key, key_hash, value
      if result.nil?
        rehash
        self[key] = value
      else
        @size += 1 if result
      end

      value
    end

    alias_method :store, :[]=

    def delete(key)
      key_hash = key.hash

      index = key_index key_hash
      if array = @entries[index] and entry = array.delete(key, key_hash)
        @size -= 1
        @entries[index] = nil if array.empty?
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

    MIN_SIZE = 16

    def self.allocate
      hash = super()
      Rubinius.privately { hash.setup }
      hash
    end

    # The +capacity+ must be a power of 2!
    def setup(capacity = MIN_SIZE)
      @capacity = capacity
      @mask = capacity - 1
      @size = 0
      @entries = Rubinius::Tuple.new capacity
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

      i = 0
      while i < @capacity
        if array = @entries[i]
          entry = array.shift
          @entries[i] = nil if array.empty?
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
