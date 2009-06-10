unless Kernel.method_defined? :const_lookup
  module Kernel
    # Implements rb_path2name. Based on code from wycats
    def const_lookup(name)
      names = name.split '::'
      names.shift if names.first.empty?
      names.inject(Object) do |m, n|
        m.const_defined?(n) ? m.const_get(n) : m.const_missing(n)
      end
    end
  end
end

MAX_KEYS = (ENV['MAX_KEY'] || 20_000).to_i
HALF_MAX_KEYS = MAX_KEYS / 2

HASH_CLASS = const_lookup(ENV['HASH_CLASS'] || "Hash")

HASH_KEYS = Array.new MAX_KEYS

(0..MAX_KEYS).each do |i|
  HASH_KEYS[i] = "key#{i}".to_sym
end

srand(1)


# Returns a random key in the range 0...max. This ensures that
# if all 0...max keys have been set, requesting a key will be
# guaranteed to match an entry.
def get_key(max = HALF_MAX_KEYS)
  HASH_KEYS[rand(max) % MAX_KEYS]
end

# Returns a random key that would not be returned by #get_key
# for use in benchmarking missing key lookup.
def get_missing_key(max = HALF_MAX_KEYS)
  HASH_KEYS[HALF_MAX_KEYS + (rand(max) % HALF_MAX_KEYS)]
end

# Returns of the maximum sizes of Hash instances to construct
# and the number of iterations to run the benchmark.
def sizes_and_iterations
  [ [5,       [20_000]],
    [10,      [10_000]],
    [50,      [1500]],
    [100,     [1000]],
    [1000,    [100]],
    [10_000,  [10]]]
end

# Returns a new instance of the Hash class being benchmarked.
def new_hash
  HASH_CLASS.new
end
