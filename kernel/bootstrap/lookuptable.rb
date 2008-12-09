# depends on: module.rb

##
# A LookupTable is similar to a Hash in that keys are used to set and
# reference values. However, unlike Hash, whether a key matches an
# entry in LookupTable is determined by using the == comparison operator
# in C code. In effect, two keys are equal if they are the same pointer.
#
# NOTE: the value used to determine the bin is the "pointer" value of
# key >> 2. This makes it possible to calculate the bin from Ruby with
# the same result as in C. For example:
# 
#   l = LookupTable.new
#   loc = :a.hash & (l.bins - 1)  # => 12
#   class LookupTable
#     def show
#       @values
#     end
#   end
#
#   l[:a] = 1
#   l.show  # => #<Tuple: nil, ..., nil, #<Bucket: @key=>:a, @value=>1,
#                         @next=>nil>, nil, nil, nil>
#
# where ... is ten "nil, " entries. This only works for Objects that
# are tagged data (see e.g. shotgun/lib/oop.h). This will NOT work with
# a String, for instance.
#
# LookupTable is intended to be used with Symbol or Fixnum keys. Internally,
# String keys are converted to Symbols. LookupTable is NOT intended to be
# used generally like Hash.

class LookupTable
  class Bucket
    def key;   @key   ; end
    def value; @value ; end
    def next;  @next  ; end
  end

  def values;  @values  ; end
  def bins;    @bins    ; end
  def size;    @entries ; end

  def self.allocate
    Ruby.primitive :lookuptable_allocate
    raise PrimitiveFailure, "LookupTable.allocate primitive failed"
  end

  def initialize(hash=nil)
    return unless hash
    hash.each do |k,v|
      self[k] = v
    end
  end

  def dup
    Ruby.primitive :lookuptable_dup
    raise PrimitiveFailure, "LookupTable#dup primitive failed"
  end

  def [](key)
    Ruby.primitive :lookuptable_aref
    raise PrimitiveFailure, "LookupTable#[] primitive failed"
  end

  def []=(key, val)
    Ruby.primitive :lookuptable_store
    raise PrimitiveFailure, "LookupTable#[]= primitive failed"
  end

  def fetch(key, return_on_failure)
    Ruby.primitive :lookuptable_fetch
    raise PrimitiveFailure, "fetch failed"
  end

  def key?(key)
    Ruby.primitive :lookuptable_has_key
    raise PrimitiveFailure, "LookupTable#key? primitive failed"
  end

  def delete(key)
    Ruby.primitive :lookuptable_delete
    raise PrimitiveFailure, "LookupTable#delete primitive failed"
  end

  def keys
    Ruby.primitive :lookuptable_keys
    raise PrimitiveFailure, "LookupTable#keys primitive failed"
  end

  def values
    Ruby.primitive :lookuptable_values
    raise PrimitiveFailure, "LookupTable#keys primitive failed"
  end

  def entries
    Ruby.primitive :lookuptable_entries
    raise PrimitiveFailure, "LookupTable#entries primitive failed"
  end

  def each
    raise LocalJumpError, "no block given" unless block_given? or @entries == 0

    ents = entries
    i = ents.start
    total = ents.start + ents.total
    while i < total
      entry = ents[i]
      yield [entry.key, entry.value]
      i += 1
    end
    self
  end

  alias_method :length,   :size
  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?,  :key?
  alias_method :clone,    :dup
end
