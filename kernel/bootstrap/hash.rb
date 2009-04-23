class Hash

  # Bucket abstracts storage and search of entries in Hash.
  # Each bin in a Hash is either +nil+, a single +Bucket+
  # instance, or the head of a chain of +Bucket+ instances.
  class Bucket
    attr_accessor :key
    attr_accessor :value
    attr_accessor :key_hash
    attr_accessor :next

    def initialize(key=nil, value=nil, key_hash=nil, nxt=nil)
      @key      = key
      @value    = value
      @key_hash = key_hash
      @next     = nxt
    end

    # Common test for checking equality of Hash keys
    def match?(key, key_hash)
      key.equal?(self.key) or (self.key_hash == key_hash and key.eql? self.key)
    end

    # Searches this chain of buckets for one matching both +key+
    # and +key_hash+. Returns +nil+ if there is no match. Calls
    # <code>#eql?</code> on +key+.
    def find(key, key_hash)
      return self if match? key, key_hash
      return unless nxt = self.next
      nxt.find key, key_hash
    end

    # Either updates an existing bucket matching +key+ and +key_hash+
    # or appends a new bucket containing +key+, +value+, +key_hash+
    # to the chain. Returns +true+ if a new bucket was added, otherwise
    # returns +false+.
    def set(key, value, key_hash)
      if match? key, key_hash
        self.value = value
        return false
      elsif nxt = self.next
        return nxt.set(key, value, key_hash)
      else
        bucket = Bucket.new key, value, key_hash
        self.next = bucket
        return true
      end
    end

    # There are three possible outcomes here:
    #
    #   1. +key+ is not found in this chain
    #   2. +key+ is the head of the chain
    #   3. +key+ is in the tail
    #
    # Returns <tt>[nil, true]</tt> in case 1. Returns <tt>[entry, nil]</tt>
    # in case 2. Returns <tt>[entry, false]</tt> in case 3.
    def delete(key, key_hash, parent = nil)
      if match? key, key_hash
        return self, nil unless parent
        parent.next = self.next
        return self, false
      elsif nxt = self.next
        return nxt.delete(key, key_hash, self)
      else
        return nil, true
      end
    end
  end

  # An external iterator that returns only bucket chains from the
  # Hash storage, never nil bins. While somewhat following the API
  # of Enumerator, it is named Iterator because it does not provide
  # <code>#each</code> and should not conflict with +Enumerator+ in
  # MRI 1.8.7+. Returned by <code>Hash#to_iter</code>.
  class Iterator
    def initialize(bins, records)
      @bins = bins
      @records = records
      @index = -1
    end

    # Returns the next object or +nil+.
    def next
      while (@index += 1) < @records
        bucket = @bins[@index]
        return bucket if bucket
      end
    end

    # Returns the index of the last object returned by <code>#next</code>.
    def index
      @index
    end

    # Resets the index of the next object to be returned by
    # <code>#next</code> to the beginning of the storage vector.
    def reset
      @index = -1
    end
  end

  # MUST be a power of 2
  MIN_SIZE = 16

  # Allocate more storage when this full
  MAX_DENSITY = 0.75

  # Resize down when this full
  MIN_DENSITY = 0.3

  # Creates a fully-formed instance of Hash.
  #--
  # @count is the number of pairs, equivalent to <code>hsh.count</code>.
  # @records is the number of entries in +@bins+.
  # @bins is the vector of storage for the bucket chains.
  #++
  def self.allocate
    h = super()

    h.__initialize__
    h
  end

  # Sets the underlying data structures.
  def __initialize__
    # We don't need the nanny checking our symbols
    @records = MIN_SIZE
    @bins = Rubinius::Tuple.new(MIN_SIZE)
    @count = 0
  end

  # Returns the storage vector for Hash. The object should provide
  # an <code>#[]</code> for accessing and <code>#[]=</code> for setting.
  def bins
    @bins
  end

  # Returns the magnitude of the storage vector (+@bins+).
  def records
    @records
  end

  # Retuns the number of items in the Hash.
  def count
    @count
  end

  # Increments the number of items in the Hash and requests
  # that the Hash be redistributed. The request will be
  # honored if the Hash's density exceeds a threshold.
  def count=(count)
    @count = count
    redistribute false
  end

  # Returns an external iterator for the bins. See +Iterator+
  def to_iter
    Iterator.new @bins, @records
  end

  # Returns a bucket for +key_hash+ from a bin. Creates a new
  # bucket if there was no bucket at that bin. The bucket may be
  # the head of a chain. Call <code>#find</code> to locate a value
  # or <code>#set</code> to add or update a value. Those methods
  # handle manipulating the bucket chain.
  def entry(key, key_hash)
    bin = entry_bin key_hash
    entry = @bins[bin]
    return entry if entry

    self.count += 1

    # recalc the bin since #count may have invoked redistribute
    @bins[entry_bin(key_hash)] = Bucket.new key, nil, key_hash
  end

  # Returns the index into the storage for +key_hash+.
  def entry_bin(key_hash)
    key_hash & (@records - 1)
  end

  # Grows or shrinks the hash storage and redistributes the entries
  # among the new bins if the entry density is outside the max/min
  # thresholds. Any Iterator instance will be invalid after a call
  # to redistribute. If +rehash+ is true, recalculate the key_hash
  # for each key.
  def redistribute(rehash = true)
    if @count >= MAX_DENSITY * @records
      new_size = @records * 2
    elsif @count < MIN_DENSITY * @records and @records > MIN_SIZE
      new_size = @records / 2
    else
      return unless rehash
      new_size = @records
    end

    i = to_iter
    @records = new_size
    @bins = Rubinius::Tuple.new @records

    while entry = i.next
      while entry
        nxt = entry.next

        entry.key_hash = entry.key.hash if rehash

        bin = entry_bin entry.key_hash
        if head = @bins[bin]
          entry.next = head
        else
          entry.next = nil
        end
        @bins[bin] = entry

        entry = nxt
      end
    end

    self
  end
end
