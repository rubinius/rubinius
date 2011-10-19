# Returns a new instance of the Hash class being benchmarked. Define before
# loading if using a class besides Hash.
unless Object.public_method_defined? :new_hash
  def hash_class
    Hash
  end

  def new_hash
    Hash.new
  end
end

# Repeatable sequence of random numbers
srand(1)

STRING = "rdnqsp uxq\nhnokjirs\nb c6rlh|4c@jcb av8\nPvunszwijhy lz  kdgy7hlKlR nzqxg\ndqldeg nm-yg vmnb mk gdrn  x"
N = STRING.size

SYM_KEYS = Array.new(10000) { |i| :"key#{i}" }
STR_KEYS = Array.new(10000) { |i| s = STRING[rand(N/4)..rand(N/2)]; 4.times { s << rand(128) }; s }

def new_sym_hash(n)
  h = new_hash
  n.times { |i| h[SYM_KEYS[i]] = i }
  h
end

def new_str_hash(n)
  h = new_hash
  n.times { |i| h[STR_KEYS[i]] = i }
  h
end
