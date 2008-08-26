class Hash

  # Bucket abstracts storage and search of entries in Hash.
  # Each bin in a Hash is either +nil+, a single +Bucket+
  # instance, or the head of a chain of +Bucket+ instances.
  #--
  # The primitives for Bucket are #at and #put. This can be
  # optimized in the compiler by converting a call to #key
  # directly into a call to #at(0).
  #++
  class Bucket < Tuple
    def initialize(key=nil, value=nil, key_hash=nil, nxt=nil)
      self.key      = key
      self.value    = value
      self.key_hash = key_hash
      self.next     = nxt
    end

    def key
      at 0
    end

    def key=(key)
      put 0, key
    end

    def value
      at 1
    end

    def value=(value)
      put 1, value
    end

    def key_hash
      at 2
    end

    def key_hash=(key_hash)
      put 2, key_hash
    end

    def next
      at 3
    end

    def next=(nxt)
      put 3, nxt
    end

    # Searches this chain of buckets for one matching both +key+
    # and +key_hash+. Returns +nil+ if there is no match. Calls
    # <code>#eql?</code> on +key+.
    #--
    # If performance needs to be enhanced, manually inline calls
    # to e.g. #key to calls to #at(2). This optimization could
    # be a compiler plugin. The same idea applies to #set.
    #++
    def find(key, key_hash)
      return self if self.key_hash == key_hash and key.eql? self.key
      return unless nxt = self.next
      nxt.find key, key_hash
    end

    # Either updates an existing bucket matching +key+ and +key_hash+
    # or appends a new bucket containing +key+, +value+, +key_hash+
    # to the chain.
    def set(key, value, key_hash)
      if self.key_hash == key_hash and key.eql? self.key
        self.value = value
        self
      elsif nxt = self.next
        nxt.set key, value, key_hash
      else
        bucket = Bucket.new key, value, key_hash
        self.next = bucket
      end
    end

    # Returns true if the entire chain would be removed. Otherwise,
    # returns false and removes the bucket containing +key+ and
    # +key_hash+ from the chain if it exists. If no bucket containing
    # +key+ and +key_hash+ exists, <code>#delete</code> does nothing.
    def delete(key, key_hash, parent = nil)
      if self.key_hash == key_hash and key.eql? self.key
        return true unless parent
        parent.next = self.next
      elsif nxt = self.next
        return nxt.delete key, key_hash, self
      end

      return false
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

  # ensure that key_hash is a Fixnum
  MAX_HASH_VALUE = 0x1fffffff

  # Creates a fully-formed instance of Hash.
  #--
  # @size is the number of pairs, equivalent to <code>hsh.size</code>.
  # @records is the number of entries in +@bins+.
  # @bins is the vector of storage for the bucket chains.
  #++
  def self.allocate
    h = super
    h.instance_variable_set :@records, MIN_SIZE
    h.instance_variable_set :@bins, Tuple.new(MIN_SIZE)
    h.instance_variable_set :@size, 0
    h
  end

  # Returns the storage vector for Hash. The object should provide
  # an <code>#[]</code> for accessing and <code>#[]=</code> for setting.
  def bins
    @bins
  end

  def records
    @records
  end

  def size
    @size
  end

  # Returns a hash for key constrained to always be a Fixnum.
  def key_hash(key)
    hash = key.hash
    hash = hash % MAX_HASH_VALUE unless hash.kind_of? Fixnum
    hash
  end

  # Returns a bucket for +key_hash+ from a bin. Creates a new
  # bucket if there was no bucket at that bin. The bucket may be
  # the head of a chain. Call <code>#find</code> to locate a value
  # or <code>#set</code> to add or update a value. Those methods
  # handle manipulating the bucket chain.
  def entry(key, key_hash)
    bin = hash & (@records - 1)
    entry = @bins.at bin
    return entry if entry
    Bucket.new key, nil, key_hash
  end
end
