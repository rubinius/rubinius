require File.dirname(__FILE__) + '/../spec_helper'

# ==, [], []=, clear, default, default=, default_proc, delete,
# delete_if, each, each_key, each_pair, each_value, empty?, fetch,
# has_key?, has_value?, include?, index, indexes, indices,
# initialize_copy, inspect, invert, key?, keys, length, member?,
# merge, merge!, rehash, reject, reject!, replace, select, shift,
# size, sort, store, to_a, to_hash, to_s, update, value?, values,
# values_at

context "Hash class method" do
  specify "[] should create a new Hash" do
    example do
      Hash[:a => 1].class
    end.should == Hash
  end
  
  specify "new with object creates a new Hash with default object" do
    example do
      @h = Hash.new(5)
      @h[:a]
    end.should == 5
  end
  
  specify "new with block creates a new Hash calling block with key for the default object" do
    example do
      @h = Hash.new { |h, k| k.kind_of?(Numeric) ? h[k] = k + 2 : h[k] = k }
      [@h[1], @h['this'], @h]
    end.should == [3, "this", {1=>3, "this"=>"this"}]
  end
end

context "Hash instance method" do
  specify "== should if other has the same default object and the same values for all keys" do
    example do
      # according to pickaxe @h != @i
      @h = Hash.new(5)
      @i = Hash.new(1)
      @j = {:a=>5}
      @k = {}
      @k[:a] = 5
      [@h == @i, @h[:a], @i[:a], @h == @h, @j == @h, @j == @k]
    end.should == [true, 5, 1, true, false, true]
  end
  
  specify "[] should return the default (immediate) value" do
    example do
      @h = Hash.new(7)
      @h[1234]
    end.should == 7
  end

  specify "[] should return the default (dynamic) value" do
    example do
      @h = Hash.new {|hsh, key| hsh[key] = (key.to_s + 'awesome')}
      @h[1234]
    end.should == "1234awesome"
  end
  
  specify "[]= should associate the key with the value and return the value" do
    example do
    end.should == "not implemented"
  end
  
  specify "clear should remove all key, value pairs" do
    example do
      @h = {:a=>2,:b=>1,:c=>1}
      @h.clear
      @h
    end.should == {}
  end

  specify "default should return the default value" do
    example do
      @h = Hash.new(5)
      [@h.default, {}.default]
    end.should == [5, nil]
  end
  
  specify "default= should set the default value" do
    example do
      @h = Hash.new
      @h.default = 99
      @h.default
    end.should == 99
  end
  
  specify "default_proc should return the block passed to Hash.new" do
    example do
      @h = Hash.new { |i| 'Paris' }
      @p= @h.default_proc
      @p.call(1)
    end.should == "Paris"
  end
  
  specify "default_proc should return nil if no block was passed to proc" do
    example do
      @h = Hash.new
      @h.default_proc
    end.should == nil
  end
  
  specify "delete should delete the entry whose key is == key" do
    example do
      @h = {:a => 5, :b => 2}
      @h.delete(:b)
      @h
    end.should == {:a=>5}
  end
  
  specify "delete should return nil if the key is not found" do
    example do
      @h = {:a => 1, :b => 10, :c => 100 }
      @h.delete(:d)
    end.should == nil
  end
  
  specify "delete_if should remove every entry for which block is true" do
    example do
    end.should == "not implemented"
  end
  
  specify "each should call block once for each entry, passing key, value" do
    example do
    end.should == "not implemented"
  end
  
  specify "each_key should call block once for each key, passing key" do
    example do
    end.should == "not implemented"
  end
  
  specify "each_pair should be a synonym for each" do
    example do
    end.should == "not implemented"
  end
  
  specify "each_value should should call block once for each key, passing value" do
    example do
    end.should == "not implemented"
  end
  
  specify "empty? should return true if block has not entries" do
    example do
    end.should == "not implemented"
  end
  
  specify "fetch should return the value for key" do
    example do
    end.should == "not implemented"
  end
  
  specify "fetch should raise IndexError if key is not found" do
    example do
    end.should == "not implemented"
  end
  
  specify "fetch with default should return default if key is not found" do
    example do
    end.should == "not implemented"
  end
  
  specify "fetch with block should return value of block if key is not found" do
    example do
    end.should == "not implemented"
  end
  
  specify "has_key? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.has_key?(:a), @h.has_key?(:b), @h.has_key?('b'), @h.has_key?(2)]
    end.should == [true, true, false, false]
  end
  
  specify "has_value? should be a synonym for value?" do
    example do
    end.should == "not implemented"
  end
  
  specify "include? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.include?(:a), @h.include?(:b), @h.include?('b'), @h.include?(2)]
    end.should == [true, true, false, false]
  end

  specify "index should return the cooresponding key for value" do
    example do
    end.should == "not implemented"
  end
  
  specify "index should return nil if the value is not found" do
    example do
    end.should == "not implemented"
  end
  
  specify "indexes should be a DEPRECATED synonym for values_at" do
    example do
    end.should == "not implemented"
  end
  
  specify "indces should be a DEPRECATED synonym for values_at" do
    example do
    end.should == "not implemented"
  end
  
  specify "initialize_copy should be a synonym for replace" do
    example do
      Hash.new.send(:initialize_copy, { :a => 1, :b => 2 })
    end.should == {:a=>1, :b=>2}
  end
  
  specify "inspect should return a string representation" do
    example do
    end.should == "not implemented"
  end
  
  specify "invert should return a new hash where keys are values and vice versa" do
    example do
    end.should == "not implemented"
  end
  
  specify "key? should return true if argument is a key" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.key?(:a), @h.key?(:b), @h.key?('b'), @h.key?(2)]
    end.should == [true, true, false, false]
  end
  
  specify "key? should return false if the key was not found" do
    example do
      @h = {5 => 7}
      @h.key?(7)
    end.should == "false"
  end

  specify "key? should return true if the key's matching value was nil" do
    example do
      @h = {:xyz => nil}
      @h.key?(:xyz)
    end.should == "true"
  end

  specify "key? should return true if the key's matching value was false" do
    example do
      @h = {:xyz => false}
      @h.key?(:xyz)
    end.should == "true"
  end

  specify "key? should return true if the key was found" do
    example do
      @h = {:xyz => 9}
      @h.key?(:xyz)
    end.should == "true"
  end

  specify "keys should return an array populated with keys" do
    example do
    end.should == "not implemented"
  end

  specify "length should return the number of entries" do
    example do
    end.should == "not implemented"
  end
  
  specify "member? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.member?(:a), @h.member?(:b), @h.member?('b'), @h.member?(2)]
    end.should == [true, true, false, false]
  end
  
  specify "merge should return a new hash by combining self with the contents of other" do
    example do
    end.should == "not implemented"
  end
  
  specify "merge with block sets any duplicate key to the value of block" do
    example do
    end.should == "not implemented"
  end
  
  specify "merge! should adds the entries from other, overwriting duplicate keys" do
    example do
    end.should == "not implemented"
  end
  
  specify "merge! with block sets any duplicate key to the value of block" do
    example do
    end.should == "not implemented"
  end
  
  specify "rehash should be provided" do
    example do
      Hash.new.respond_to?(:rehash)
    end.should == true
  end
  
  specify "reject should be equivalent to hsh.dup.delete_if" do
    example do
    end.should == "not implemented"
  end
  
  specify "reject! should return nil if no changes were made" do
    example do
    end.should == "not implemented"
  end
  
  specify "replace should replace the contents of self with other" do
    example do
    end.should == "not implemented"
  end
  
  specify "select should return a new hash of entries for which block is true" do
    example do
    end.should == "not implemented"
  end
  
  specify "shift should remove an entry from hash and return it in a two-element array" do
    example do
    end.should == "not implemented"
  end
  
  specify "size should be a synonym for length" do
    example do
    end.should == "not implemented"
  end
  
  specify "sort should convert self to a nested array of [key, value] arrays and sort with Array#sort" do
    example do
    end.should == "not implemented"
  end
  
  specify "sort with block should use block to sort array" do
    example do
    end.should == "not implemented"
  end

  specify "store should " do
    example do
    end.should == "not implemented"
  end
  
  specify "to_a should " do
    example do
    end.should == "not implemented"
  end
  
  specify "to_hash should " do
    example do
    end.should == "not implemented"
  end
  
  specify "to_s should " do
    example do
    end.should == "not implemented"
  end
  
  specify "update should " do
    example do
    end.should == "not implemented"
  end
  
  specify "value? should " do
    example do
    end.should == "not implemented"
  end
  
  specify "values should " do
    example do
    end.should == "not implemented"
  end
  
  specify "values_at should " do
    example do
    end.should == "not implemented"
  end
end
