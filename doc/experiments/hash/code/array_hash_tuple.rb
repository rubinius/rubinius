module Rubinius
  class ArrayHash < Hash
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
            if @tuple[@tuple_index]
              return @tuple[@tuple_index+1], @tuple[@tuple_index+2]
            end
          end
        end

        while (@index += 1) < @capacity
          if @tuple = @entries[@index]
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

    def match?(tuple, index, key, key_hash)
      return unless kh = tuple[index]
      k = tuple[index+1]
      key.equal?(k) or (kh == key_hash and key.eql? k)
    end

    def find_entry(key)
      key_hash = key.hash

      if tuple = @entries[key_index(key_hash)]
        index = 0
        total = tuple.size
        while index < total
          if match? tuple, index, key, key_hash
            return tuple[index+1], tuple[index+2]
          end
          index += 3
        end
      end
    end
    private :find_entry

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
      setup @capacity * 2

      i = 0
      capacity = entries.size
      while i < capacity
        if other = entries[i]
          j = 0
          other_total = other.size
          while j < other_total
            if key_hash = other[j]
              index = key_index key_hash
              if tuple = @entries[index]
                k = 0
                total = tuple.size
                while k < total
                  unless tuple[k]
                    tuple.copy_from other, j, 3, k
                    break
                  end
                  k += 3
                end

                if k >= total
                  new_total = total * 2
                  if new_total > MAX_TUPLE
                    return redistribute entries
                  else
                    new_tuple = Tuple.new new_total
                    new_tuple.copy_from tuple, 0, total, 0
                    new_tuple.copy_from other, j, 3, total
                    @entries[index] = new_tuple
                  end
                end
              else
                tuple = Tuple.new MIN_TUPLE
                tuple.copy_from other, j, 3, 0
                @entries[index] = tuple
              end
            end
            j += 3
          end
        end

        i += 1
      end

      self
    end

    def new_entry(tuple, index, key, key_hash, value)
      if key.kind_of? String
        key = key.dup
        key.freeze
      end

      tuple[index]   = key_hash
      tuple[index+1] = key
      tuple[index+2] = value
    end

    def []=(key, value)
      key_hash = key.hash
      index = key_index key_hash

      # If the slot is empty, add a new tuple and set the entry.
      unless tuple = @entries[index]
        tuple = Tuple.new MIN_TUPLE
        new_entry tuple, 0, key, key_hash, value
        @entries[index] = tuple
        @size += 1
        return value
      end

      # Check for a match on an existing entry.
      slot = nil
      i = 0
      total = tuple.size
      while i < total
        if tuple[i]
          if match? tuple, i, key, key_hash
            return tuple[i+2] = value
          end
        else
          slot = i unless slot
        end
        i += 3
      end

      # Did not match an existing entry, insert if there is room
      # or resize the tuple. If resizing exceeds the max tuple
      # size, redistribute the Hash.
      if slot
        @size += 1
        new_entry tuple, slot, key, key_hash, value
      else
        new_total = total * 2
        unless new_total > MAX_TUPLE
          new_tuple = Tuple.new new_total
          new_tuple.copy_from tuple, 0, total, 0
          new_entry new_tuple, total, key, key_hash, value
          @entries[index] = new_tuple
          @size += 1
        else
          redistribute @entries
          self[key] = value
        end
      end

      value
    end

    alias_method :store, :[]=

    def delete(key)
      key_hash = key.hash

      found = false
      other = false
      value = nil
      index = key_index key_hash
      if tuple = @entries[index]
        i = 0
        total = tuple.size
        while i < total
          if tuple[i]
            if !found and match? tuple, i, key, key_hash
              tuple[i] = nil
              value = tuple[i+2]
              @size -= 1
              found = true
            else
              other = true
            end
          end
          i += 3
        end
      end

      @entries[index] = nil unless other
      return value if found

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
    MIN_TUPLE = 9
    MAX_TUPLE = 18

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
        yield entry[0], entry[1]
      end
      self
    end

    alias_method :each_pair, :each_item

    def shift
      return default(nil) if empty?

      i = 0
      while i < @capacity
        if tuple = @entries[i]
          found = false
          other = false
          key = nil
          value = nil

          j = 0
          total = tuple.size
          while j < total
            if tuple[j]
              if !found
                tuple[j] = nil
                key = tuple[j+1]
                value = tuple[j+2]
                found = true
              else
                other = true
              end
            end
            j += 3
          end

          @entries[i] = nil unless other
          @size -= 1
          return key, value
        end

        i += 1
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
