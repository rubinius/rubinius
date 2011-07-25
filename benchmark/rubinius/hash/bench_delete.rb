require 'benchmark'
require 'benchmark/ips'

require File.expand_path('../setup', __FILE__)

def hash_delete_missing(t, h, k)
  i = 0
  while i < t
    h.delete k
    i += 1
  end
end

def hash_delete(t, h, k)
  i = 0
  while i < t
    h.delete k
    # add the key back so we actually measure deleting it
    h[k] = 1
    i += 1
  end
end

def hash_delete_all(t, h, keys)
  i = 0
  while i < t
    hd = h.dup
    j = 0
    n = keys.size
    while j < n
      hd.delete keys[j]
      j += 1
    end
    i += 1
  end
end

h_sym = {}
h_str = {}
sym = {}
str = {}

[10, 100, 1000, 10000].each do |n|
  h_sym[n] = new_sym_hash n
  h_str[n] = new_str_hash n
  sym[n] = SYM_KEYS[0, n]
  str[n] = STR_KEYS[0, n]
end

Benchmark.ips do |x|
  mk_sym = :"key#{SYM_KEYS.size+1}"
  ek_sym = SYM_KEYS[0]

  x.report("delete sym missing 10") { |t| hash_delete_missing t, h_sym[10], mk_sym }
  x.report("delete sym missing 100") { |t| hash_delete_missing t, h_sym[100], mk_sym }
  x.report("delete sym missing 1000") { |t| hash_delete_missing t, h_sym[1000], mk_sym }
  x.report("delete sym missing 10000") { |t| hash_delete_missing t, h_sym[10000], mk_sym }

  x.report("delete sym existing 10") { |t| hash_delete t, h_sym[10], ek_sym }
  x.report("delete sym existing 100") { |t| hash_delete t, h_sym[100], ek_sym }
  x.report("delete sym existing 1000") { |t| hash_delete t, h_sym[1000], ek_sym }
  x.report("delete sym existing 10000") { |t| hash_delete t, h_sym[10000], ek_sym }

  x.report("delete sym all 10") { |t| hash_delete_all t, h_sym[10], sym[10] }
  x.report("delete sym all 100") { |t| hash_delete_all t, h_sym[100], sym[100] }
  x.report("delete sym all 1000") { |t| hash_delete_all t, h_sym[1000], sym[1000] }
  x.report("delete sym all 10000") { |t| hash_delete_all t, h_sym[10000], sym[10000] }

  mk_str = STR_KEYS.first.dup
  mk_str << mk_str.reverse
  ek_str = STR_KEYS[0]

  x.report("delete str missing 10") { |t| hash_delete_missing t, h_str[10], mk_str }
  x.report("delete str missing 100") { |t| hash_delete_missing t, h_str[100], mk_str }
  x.report("delete str missing 1000") { |t| hash_delete_missing t, h_str[1000], mk_str }
  x.report("delete str missing 10000") { |t| hash_delete_missing t, h_str[10000], mk_str }

  x.report("delete str existing 10") { |t| hash_delete t, h_str[10], ek_str }
  x.report("delete str existing 100") { |t| hash_delete t, h_str[100], ek_str }
  x.report("delete str existing 1000") { |t| hash_delete t, h_str[1000], ek_str }
  x.report("delete str existing 10000") { |t| hash_delete t, h_str[10000], ek_str }

  x.report("delete str all 10") { |t| hash_delete_all t, h_str[10], str[10] }
  x.report("delete str all 100") { |t| hash_delete_all t, h_str[100], str[100] }
  x.report("delete str all 1000") { |t| hash_delete_all t, h_str[1000], str[1000] }
  x.report("delete str all 10000") { |t| hash_delete_all t, h_str[10000], str[10000] }
end
