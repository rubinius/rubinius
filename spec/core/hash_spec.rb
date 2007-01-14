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
      Hash.new(5)[:a]
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
      Hash.new(7)[1234]
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
      @h = { :a => 1 }
      [@h[:b] = 2, @h]
    end.should == [2, {:b=>2, :a=>1}]
  end
  
  specify "clear should remove all key, value pairs" do
    example do
      {:a=>2,:b=>1,:c=>1}.clear
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
      Hash.new.default_proc
    end.should == nil
  end
  
  specify "delete should delete the entry whose key is == key" do
    example do
      @h = {:a => 5, :b => 2}
      [@h.delete(:b), @h]
    end.should == [2, { :a => 5 }]
  end
  
  specify "delete should return nil if the key is not found" do
    example do
      {:a => 1, :b => 10, :c => 100 }.delete(:d)
    end.should == nil
  end
  
  specify "delete_if should remove every entry for which block is true" do
    example do
      @h = { :a => 1, :b => 2, :c => 3, :d => 4 }
      [@h.delete_if { |k,v| v % 2 == 1 }, @h]
    end.should == [{ :b => 2, :d => 4 }, { :b => 2, :d => 4 }]
  end
  
  specify "each should call block once for each entry, passing key, value" do
    example do
      @h = {}
      { :a, 1, :b, 2, :c, 3, :d, 5 }.each { |k,v| @h[k.to_s] = v.to_s }
      @h
    end.should == {"a" => "1", "b" => "2", "c" => "3", "d" => "5" }
  end
  
  specify "each_key should call block once for each key, passing key" do
    example do
      @h = {}
      { 1 => -1, 2 => 2, 3 => -3, 4 => 4 }.each_key { |k| @h[k] = k }
      @h
    end.should == { 1 => 1, 2 => 2, 3 => 3, 4 => 4 }
  end
  
  specify "each_pair should be a synonym for each" do
    example do
      @a = []
      { :a, 1, :b, 2, :c, 3, :d, 5 }.each_pair { |k,v| @a << "#{k} => #{v}" }
      @a.sort
    end.should == ["a => 1", "b => 2", "c => 3", "d => 5"]
  end
  
  specify "each_value should should call block once for each key, passing value" do
    example do
      @h = {}
      { :a => -5, :b => -3, :c => -2, :d => -1 }.each_value { |v| @h[v.abs] = v }
      @h
    end.should == { 5 => -5, 1 => -1, 2 => -2, 3 => -3 }
  end
  
  specify "empty? should return true if block has not entries" do
    example do
      [{}.empty?, { 1 => 1 }.empty?]
    end.should == [true, false]
  end
  
  specify "fetch should return the value for key" do
    example do
      { :a => 1, :b => -1 }.fetch(:b)
    end.should == -1
  end
  
  specify "fetch should raise IndexError if key is not found" do
    example do
      begin
        {}.fetch(:a)
      rescue IndexError
        @a = 'key not found'
      end
      @a
    end.should == "key not found"
  end
  
  specify "fetch with default should return default if key is not found" do
    example do
      {}.fetch(:a, 'not here!')
    end.should == "not here!"
  end
  
  specify "fetch with block should return value of block if key is not found" do
    example do
      {}.fetch('a') { |k| k + '!' }
    end.should == "a!"
  end
  
  specify "has_key? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.has_key?(:a), @h.has_key?(:b), @h.has_key?('b'), @h.has_key?(2)]
    end.should == [true, true, false, false]
  end
  
  specify "has_value? should be a synonym for value?" do
    example do
      [{ :a => :b }.has_value?(:a), { 1 => 2 }.has_value?(2)]
    end.should == [false, true]
  end
  
  specify "include? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.include?(:a), @h.include?(:b), @h.include?('b'), @h.include?(2)]
    end.should == [true, true, false, false]
  end

  specify "index should return the cooresponding key for value" do
    example do
      { 2 => 'a', 1 => 'b' }.index('b')
    end.should == 1
  end
  
  specify "index should return nil if the value is not found" do
    example do
      { :a => -1, :b => 3.14, :c => 2.718 }.index(1)
    end.should == nil
  end
  
  specify "indexes should be a DEPRECATED synonym for values_at" do
    example do
      {:a => 9, :b => 'a', :c => -10, :d => nil}.indexes(:a, :d, :b)
    end.should == [9, nil, 'a']
  end
  
  specify "indices should be a DEPRECATED synonym for values_at" do
    example do
      {:a => 9, :b => 'a', :c => -10, :d => nil}.indices(:a, :d, :b)
    end.should == [9, nil, 'a']
  end
  
  specify "initialize_copy should be a synonym for replace" do
    example do
      Hash.new.send(:initialize_copy, { :a => 1, :b => 2 })
    end.should == {:a=>1, :b=>2}
  end
  
  specify "inspect should return a string representation" do
    example do
      { :a => 0, :b => -2, :c => 4, :d => -6 }.inspect.class
    end.should == String
  end
  
  specify "invert should return a new hash where keys are values and vice versa" do
    example do
      { 1 => 'a', 2 => 'b', 3 => 'c' }.invert
    end.should == { 'a' => 1, 'b' => 2, 'c' => 3 }
  end
  
  specify "key? should return true if argument is a key" do
    example do
      @h = { :a => 1, :b => 2, :c => 3 }
      [@h.key?(:a), @h.key?(:b), @h.key?('b'), @h.key?(2)]
    end.should == [true, true, false, false]
  end
  
  specify "key? should return false if the key was not found" do
    example do
      { 5 => 7 }.key?(7)
    end.should == false
  end

  specify "key? should return true if the key's matching value was nil" do
    example do
      { :xyz => nil }.key?(:xyz)
    end.should == true
  end

  specify "key? should return true if the key's matching value was false" do
    example do
      { :xyz => false }.key?(:xyz)
    end.should == true
  end

  specify "key? should return true if the key was found" do
    example do
      { :xyz => 9 }.key?(:xyz)
    end.should == true
  end

  specify "keys should return an array populated with keys" do
    example do
      { 1 => 2, 2 => 4, 4 => 8 }.keys
    end.should == [1, 2, 4]
  end

  specify "length should return the number of entries" do
    example do
      [{ :a => 1, :b => 'c' }.length, {}.length]
    end.should == [2, 0]
  end
  
  specify "member? should be a synonym for key?" do
    example do
      @h = {:a => 1, :b => 2, :c => 3}
      [@h.member?(:a), @h.member?(:b), @h.member?('b'), @h.member?(2)]
    end.should == [true, true, false, false]
  end
  
  specify "merge should return a new hash by combining self with the contents of other" do
    example do
      { 1, :a, 2, :b, 3, :c }.merge(:a => 1, :c => 2)
    end.should == { :c=> 2, 1 => :a, 2 => :b, :a => 1, 3 => :c }
  end
  
  specify "merge with block sets any duplicate key to the value of block" do
    example do
      { :a => 2, :b => 1 }.merge(:a => -2, :c => -3) { |k,v| -9 }
    end.should == { :c => -3, :b => 1, :a => -9 }
  end
  
  specify "merge! should adds the entries from other, overwriting duplicate keys" do
    example do
      @h = { :_1 => 'a', :_2 => '3' }
      [@h.merge!(:_1 => '9', :_9 => 2), @h]
    end.should == [{ :_1 => "9", :_2 => "3", :_9 => 2 }, {:_1 => "9", :_2 => "3", :_9 => 2 }]
  end
  
  specify "merge! with block sets any duplicate key to the value of block" do
    example do
      @h = { :a => 2, :b => -1 }
      [@h.merge!(:a => -2, :c => 1) { |k,v| 3.14 }, @h]
    end.should == [{:c => 1, :b => -1, :a => 3.14 }, {:c => 1, :b => -1, :a => 3.14 }]
  end
  
  specify "rehash should be provided" do
    example do
      Hash.new.respond_to?(:rehash)
    end.should == true
  end
  
  specify "reject should be equivalent to hsh.dup.delete_if" do
    example do
      @h = { :a => 'a', :b => 'b', :c => 'd' }
      [@h.reject { |k,v| k == 'd' }, @h]
    end.should == [{:c=>"d", :b=>"b", :a=>"a"}, {:c=>"d", :b=>"b", :a=>"a"}]
  end
  
  specify "reject! should return nil if no changes were made" do
    example do
      { :a => 1 }.reject! { |k,v| v > 1 }
    end.should == nil
  end
  
  specify "replace should replace the contents of self with other" do
    example do
      @h = { :a => 1, :b => 2 }
      @h.replace(:c => -1, :d => -2)
    end.should == { :c => -1, :d => -2 }
  end
  
  specify "select should return a new hash of entries for which block is true" do
    example do
      @h = { :a => 9, :c => 4, :b => 5, :d => 2 }.select { |k,v| v % 2 == 0 }
      @h.sort { |a,b| a.to_s <=> b.to_s }
    end.should == [[:c, 4], [:d, 2]]
  end
  
  specify "shift should remove an entry from hash and return it in a two-element array" do
    example do
      @h = { :a => 2 }
      [@h.shift, @h]
    end.should == [[:a, 2], { }]
  end
  
  specify "size should be a synonym for length" do
    example do
      [{ :a => 1, :b => 'c' }.size, {}.size]
    end.should == [2, 0]
  end
  
  specify "sort should convert self to a nested array of [key, value] arrays and sort with Array#sort" do
    example do
      { 'a' => 'b', '1' => '2', 'b' => 'a' }.sort
    end.should == [["1", "2"], ["a", "b"], ["b", "a"]]
  end
  
  specify "sort with block should use block to sort array" do
    example do
      { 1 => 2, 2 => 9, 3 => 4 }.sort { |a,b| b <=> a }
    end.should == [[3, 4], [2, 9], [1, 2]]
  end

  specify "store should be a synonym for []=" do
    example do
      @h = { :a => 1 }
      [@h[:b] = 2, @h]
    end.should == [2, {:b=>2, :a=>1}]
  end
  
  specify "to_a should return a nested array of [key, value] arrays" do
    example do
      { :a => 1, 1 => :a, 3 => :b, :b => 5 }.to_a.sort { |a,b| a.to_s <=> b.to_s }
    end.should == [[1, :a], [3, :b], [:a, 1], [:b, 5]]
  end
  
  specify "to_hash should should return self" do
    example do
      @h = {}
      @h.to_hash.eql?(@h)
    end.should == true
  end
  
  specify "to_s should return a string by calling Hash#to_a and using Array#join with default separator" do
    example do
      { :fun => 'fun', 'fun' => :fun }.to_s
    end.should == 'funfunfunfun'
  end
  
  specify "update should be a synonym for merge!" do
    example do
      @h = { :_1 => 'a', :_2 => '3' }
      [@h.update(:_1 => '9', :_9 => 2), @h]
    end.should == [{ :_1 => "9", :_2 => "3", :_9 => 2 }, {:_1 => "9", :_2 => "3", :_9 => 2 }]
  end
  
  specify "update with block should be a synonym for merge!" do
    example do
      @h = { :a => 2, :b => -1 }
      [@h.update(:a => -2, :c => 1) { |k,v| 3.14 }, @h]
    end.should == [{:c => 1, :b => -1, :a => 3.14 }, {:c => 1, :b => -1, :a => 3.14 }]
  end
  
  specify "value? should be a synonym for has_value?" do
    example do
      [{ :a => :b }.has_value?(:a), { 1 => 2 }.has_value?(2)]
    end.should == [false, true]
  end
  
  specify "values should return an array of values" do
    example do
      { 1 => :a, 'a' => :a, 'the' => 'lang'}.values
    end.should == [:a, :a, "lang"]
  end
  
  specify "values_at should return an array of values for the given keys" do
    example do
      {:a => 9, :b => 'a', :c => -10, :d => nil}.values_at(:a, :d, :b)
    end.should == [9, nil, 'a']
  end
end
