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
      def initialize(capacity, entries1, entries2)
        @capacity = capacity
        @entries1 = entries1
        @entries2 = entries2
        @entries  = @entries1
        @index    = -1
      end

      # Returns the next object or +nil+.
      def next
        while (@index += 1) < @capacity
          entry = @entries[@index]
          return entry if entry
        end

        return if @entries.equal? @entries2

        @index = -1
        @entries = @entries2
        self.next
      end
    end

    attr_reader :size

    alias_method :length, :size

    def key_index1(key_hash)
      key_hash & @mask
    end
    private :key_index1

    def key_index2(key_hash)
      (key_hash ^ 0xaaebc93) & @mask
    end
    private :key_index2

    def find_entry(key)
      key_hash = key.hash
      entry = @entries1[key_index1(key_hash)]
      return entry if entry and entry.match? key, key_hash

      entry = @entries2[key_index2(key_hash)]
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
      setup @capacity * 2

      while entry = i.next
        insert entry.key, entry.value, false
      end

      self
    end

    def insert(key, value, updating)
      rehash unless @size < @capacity

      key_hash = key.hash
      index = key_index1 key_hash
      unless entry = @entries1[index]
        @entries1[index] = Entry.new key, key_hash, value
        @size += 1
        return
      end

      if updating
        if entry.match? key, key_hash
          entry.value = value
          return
        end

        index2 = key_index2 key_hash
        if entry2 = @entries2[index2] and entry2.match? key, key_hash
          entry2.value = value
          return
        end
      end

      new_entry = Entry.new key, key_hash, value

      i = 1
      while i < @capacity
        # Boot the found entry from the nest and take it over.
        @entries1[index] = new_entry
        new_entry = entry

        # Repeat with the booted entry as the new_entry
        index = key_index2 new_entry.key_hash
        unless entry = @entries2[index]
          @entries2[index] = new_entry
          @size += 1
          return
        end

        @entries2[index] = new_entry
        new_entry = entry

        # If there is no entry for this key, set the entry.
        index = key_index1 new_entry.key_hash
        unless entry = @entries1[index]
          @entries1[index] = new_entry
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
      if entry = @entries1[index] and entry.match? key, key_hash
        @entries1[index] = nil
        @size -= 1
        return entry.value
      end

      index = key_index2 key_hash
      if entry = @entries2[index] and entry.match? key, key_hash
        @entries2[index] = nil
        @size -= 1
        return entry.value
      end

      return yield(key) if block_given?
    end

    # TODO: remove when merged
    def print
      puts "Size: #{@capacity}, Used: #{@size}, Entries: #{@entries}"

      found = 0
      puts "Table 1:"
      @table1.entries.each_with_index do |ent, idx|
        if ent
          found += 1
          puts "#{idx}: #{ent.key.inspect} => #{ent.value.inspect}"
        else
          puts "#{idx}:"
        end
      end

      puts "Table 2:"
      @table2.entries.each_with_index do |ent, idx|
        if ent
          found += 1
          puts "#{idx}: #{ent.key.inspect} => #{ent.value.inspect}"
        else
          puts "#{idx}:"
        end
      end

      puts "Found: #{found}"
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

    def self.allocate
      hash = super()
      Rubinius.privately { hash.setup }
      hash
    end

    # +capacity+ must be a power of 2!
    def setup(capacity = 8)
      @capacity = capacity
      @mask = capacity - 1
      @size = 0
      @entries1 = Rubinius::Tuple.new capacity
      @entries2 = Rubinius::Tuple.new capacity
    end
    private :setup

    def clear
      setup
      self
    end

    def to_iter
      Iterator.new @capacity, @entries1, @entries2
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
        if entry = @entries1[i]
          @entries1[i] = nil
          @size -= 1
          return entry.key, entry.value
        end

        if entry = @entries2[i]
          @entries2[i] = nil
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
