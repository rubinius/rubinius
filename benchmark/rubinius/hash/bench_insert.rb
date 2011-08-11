require 'benchmark'
require 'benchmark/ips'

require File.expand_path('../setup', __FILE__)

def hash_insert(t, a)
  i = 0
  while i < t
    h = new_hash
    a.each { |k| h[k] = 1 }
    i += 1
  end
end

def hash_insert_existing(t, h, a)
  i = 0
  while i < t
    a.each { |k| h[k] = 1 }
    i += 1
  end
end

sym = {}
str = {}

[10, 100, 1000, 10000].each do |n|
  sym[n] = SYM_KEYS[0, n]
  str[n] = STR_KEYS[0, n]
end

Benchmark.ips do |x|
  x.report("insert sym 10") { |t| hash_insert t, sym[10] }
  x.report("insert sym 100") { |t| hash_insert t, sym[100] }
  x.report("insert sym 1000") { |t| hash_insert t, sym[1000] }
  x.report("insert sym 10000") { |t| hash_insert t, sym[10000] }

  x.report("insert sym existing 10") { |t| hash_insert_existing t, new_hash, sym[10] }
  x.report("insert sym existing 10000") { |t| hash_insert_existing t, new_hash, sym[10000] }

  x.report("insert str 10") { |t| hash_insert t, str[10] }
  x.report("insert str 100") { |t| hash_insert t, str[100] }
  x.report("insert str 1000") { |t| hash_insert t, str[1000] }
  x.report("insert str 10000") { |t| hash_insert t, str[10000] }

  x.report("insert str existing 10") { |t| hash_insert_existing t, new_hash, str[10] }
  x.report("insert str existing 10000") { |t| hash_insert_existing t, new_hash, str[10000] }
end
