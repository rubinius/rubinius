require 'benchmark'
require 'benchmark/ips'

require File.expand_path('../setup', __FILE__)

# Note: in 1.8, Hash#select returns an Array while in 1.9 it returns a Hash, so
# be sure that this benchmark is comparing apples to apples.

def hash_select(t, h, bool, mod)
  i = 0
  while i < t
    h.select { |k,v| bool ^= mod }
    i += 1
  end
end

h_sym = {}
h_str = {}

[10, 10000].each do |n|
  h_sym[n] = new_sym_hash n
  h_str[n] = new_str_hash n
end

Benchmark.ips do |x|
  x.report("select sym all 10") { |t| hash_select t, h_sym[10], true, false }
  x.report("select sym all 10000") { |t| hash_select t, h_sym[10000], true, false }

  x.report("select sym none 10") { |t| hash_select t, h_sym[10], false, false }
  x.report("select sym none 10000") { |t| hash_select t, h_sym[10000], false, false }

  x.report("select sym half 10") { |t| hash_select t, h_sym[10], true, true }
  x.report("select sym half 10000") { |t| hash_select t, h_sym[10000], true, true }

  x.report("select str all 10") { |t| hash_select t, h_str[10], true, false }
  x.report("select str all 10000") { |t| hash_select t, h_str[10000], true, false }

  x.report("select str none 10") { |t| hash_select t, h_str[10], false, false }
  x.report("select str none 10000") { |t| hash_select t, h_str[10000], false, false }

  x.report("select str half 10") { |t| hash_select t, h_str[10], true, true }
  x.report("select str half 10000") { |t| hash_select t, h_str[10000], true, true }
end
