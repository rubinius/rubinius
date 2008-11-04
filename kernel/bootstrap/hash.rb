# depends on: module.rb symbol.rb string.rb lookuptable.rb

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

    # Searches this chain of buckets for one matching both +key+
    # and +key_hash+. Returns +nil+ if there is no match. Calls
    # <code>#eql?</code> on +key+.
    def find(key, key_hash)
      return self if self.key_hash == key_hash and key.eql? self.key
      return unless nxt = self.next
      nxt.find key, key_hash
    end

    # Either updates an existing bucket matching +key+ and +key_hash+
    # or appends a new bucket containing +key+, +value+, +key_hash+
    # to the chain. Returns +true+ if a new bucket was added, otherwise
    # returns +false+.
    def set(key, value, key_hash)
      if self.key_hash == key_hash and key.eql? self.key
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

    # Returns +false+ if the bucket was found and deleted and the head
    # of the chain is unchanged. Returns +nil+ if the head of the
    # chain should be replaced with <code>head.next</code>. Returns
    # +true+ if the bucket was not found.
    # FIX: cryptic. return a symbol for all 3 cases
    def delete(k, k_hash, parent = nil)
      # identity wins. see rb_any_cmp in hash.c in mri for clues.
      if k.equal?(self.key) or (self.key_hash == k_hash and k.eql? self.key)
        return nil unless parent
        parent.next = self.next
        return false
      elsif nxt = self.next
        return nxt.delete(k, k_hash, self)
      else
        return true
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

  # Ensure that key_hash is a Fixnum
  MAX_HASH_VALUE = 0x1fffffff

  # Allocate more storage when this full
  MAX_DENSITY = 0.75

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
    set_instance_variable :@records, MIN_SIZE
    set_instance_variable :@bins, Tuple.new(MIN_SIZE)
    set_instance_variable :@count, 0
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

  # Grows the hash storage and redistributes the entries among
  # the new bins if the entry density is above a threshold. Any
  # Iterator instance will be invalid after a call to redistribute.
  # If +rehash+ is true, recalculate the key_hash for each key.
  def redistribute(rehash = true)
    resize = @count >= MAX_DENSITY * @records
    return unless rehash or resize

    i = to_iter
    @records *= 2 if resize
    @bins = Tuple.new @records

    while entry = i.next
      while entry
        nxt = entry.next

        entry.key_hash = key_hash entry.key if rehash

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
