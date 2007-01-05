require File.dirname(__FILE__) + '/../spec_helper'

# ==, [], []=, clear, default, default=, default_proc, delete,
# delete_if, each, each_key, each_pair, each_value, empty?, fetch,
# has_key?, has_value?, include?, index, indexes, indices,
# initialize_copy, inspect, invert, key?, keys, length, member?,
# merge, merge!, rehash, reject, reject!, replace, select, shift,
# size, sort, store, to_a, to_hash, to_s, update, value?, values,
# values_at

context "Hash instance method" do
  specify "key? should return true if argument is a key" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      p [@h.key?(:a), @h.key?(:b), @h.key?('b'), @h.key?(2)]
    end.should == '[true, true, false, false]'
  end
end
