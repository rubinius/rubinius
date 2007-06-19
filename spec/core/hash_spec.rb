require File.dirname(__FILE__) + '/../spec_helper'

# ==, [], []=, clear, default, default=, default_proc, delete,
# delete_if, each, each_key, each_pair, each_value, empty?, fetch,
# has_key?, has_value?, include?, index, indexes, indices,
# initialize_copy, inspect, invert, key?, keys, length, member?,
# merge, merge!, rehash, reject, reject!, replace, select, shift,
# size, sort, store, to_a, to_hash, to_s, update, value?, values,
# values_at

class MyHash < Hash; end

context "Hash class method" do
  specify "[] creates a Hash; values can be provided as the argument list" do
    Hash[:a, 1, :b, 2].should == {:a => 1, :b => 2}
    Hash[].should == {}
  end

  specify "[] creates a Hash; values can be provided as one single hash" do
    Hash[:a => 1, :b => 2].should == {:a => 1, :b => 2} 
    Hash[{1 => 2, 3 => 4}].should == {1 => 2, 3 => 4}
    Hash[{}].should == {}
  end

  specify "[] raises on odd parameter list count" do
    should_raise(ArgumentError) { Hash[1, 2, 3] }
  end
 
  specify "[] raises when mixing argument styles" do
    should_raise(ArgumentError) { Hash[1, 2, {3 => 4}] }
    Hash[1, 2, 3, {3 => 4}].should == {1 => 2, 3 => {3 => 4}}
  end
  
  specify "[] shouldn't call to_hash" do
    obj = Object.new
    def obj.to_hash() { 1 => 2, 3 => 4 } end
    should_raise(ArgumentError) { Hash[obj] }
  end

  specify "[] should always return an instance of the class it's called on" do
    Hash[MyHash[1, 2]].class.should == Hash
    MyHash[Hash[1, 2]].class.should == MyHash
  end

  specify "new with default argument creates a new Hash with default object" do
    Hash.new(5).default.should == 5
    Hash.new({}).default.should == {}
  end

  specify "new with default argument shouldn't create a copy of the default" do
    str = "foo"
    Hash.new(str).default.equal?(str).should == true
  end
  
  specify "new with block creates a Hash with a default_proc" do
    Hash.new.default_proc.should == nil
    
    hash = Hash.new { |x| "Answer to #{x}" }
    hash.default_proc.call(5).should == "Answer to 5"
    hash.default_proc.call("x").should == "Answer to x"
  end
  
  specify "new with default argument and default block should raise" do
    should_raise(ArgumentError) { Hash.new(5) { 0 } }
    should_raise(ArgumentError) { Hash.new(nil) { 0 } }
  end
end

context "Hash instance methods" do
  specify "== is true if they have the same number of keys and each key-value pair matches" do
    Hash.new(5).should == Hash.new(1)
    Hash.new {|h, k| 1}.should == Hash.new {}
    Hash.new {|h, k| 1}.should == Hash.new(2)
    
    a = {:a => 5}
    b = {}

    a.should_not == b

    b[:a] = 5

    a.should == b

    c = Hash.new {|h, k| 1}
    d = Hash.new {}
    c[1] = 2
    d[1] = 2
    c.should == d
  end
  
  specify "== should be instantly false when the numbers of keys differ" do
    obj = Object.new
    obj.should_not_receive(:==)
    obj.should_not_receive(:eql?)

    {}.should_not == { obj => obj }
    { obj => obj }.should_not == {}
  end
  
  specify "== should compare keys with eql? semantics" do
    { 1.0 => "x" }.should == { 1.0 => "x" }
    { 1 => "x" }.should_not == { 1.0 => "x" }
    { 1.0 => "x" }.should_not == { 1 => "x" }
  end

  specify "== should first compare keys via hash" do
    # Can't use should_receive because it uses hash internally
    x = Object.new
    def x.hash() freeze; 0 end
    y = Object.new
    def y.hash() freeze; 0 end

    { x => 1 } == { y => 1 }
    x.frozen?.should == true
    y.frozen?.should == true
  end
    
  specify "== shouldn't compare keys with different hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = Object.new
    def x.eql?(o) raise("Shouldn't receive eql?") end
    y = Object.new
    def y.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() freeze; 0 end
    def y.hash() freeze; 1 end

    { x => 1 }.should_not == { y => 1 }
    x.frozen?.should == true
    y.frozen?.should == true
  end    
    
  specify "== should compare keys with matching hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    a = Array.new(2) do 
      obj = Object.new

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; false; end
        return 0
      end

      obj
    end

    { a[0] => 1 }.should_not == { a[1] => 1 }
    a[0].tainted?.should == true
    a[1].tainted?.should == true

    a = Array.new(2) do 
      obj = Object.new

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; true; end
        return 0
      end

      obj
    end

    { a[0] => 1 }.should == { a[1] => 1 }
    a[0].tainted?.should == true
    a[1].tainted?.should == true
  end
  
  specify "== should compare values with == semantics" do
    { "x" => 1.0 }.should == { "x" => 1 }
  end
  
  specify "== shouldn't compare values when keys don't match" do
    value = Object.new
    value.should_not_receive(:==)
    { 1 => value }.should_not == { 2 => value }
  end
  
  specify "== should compare values when keys match" do
    x = Object.new
    y = Object.new
    def x.==(o) freeze; false; end
    def y.==(o) freeze; false; end
    { 1 => x }.should_not == { 1 => y }
    # There is no order
    (x.frozen? | y.frozen?).should == true

    x = Object.new
    y = Object.new
    def x.==(o) freeze; true; end
    def y.==(o) freeze; true; end
    { 1 => x }.should == { 1 => y }
    # There is no order
    (x.frozen? | y.frozen?).should == true
  end

  specify "== should ignore hash class differences" do
    h = { 1 => 2, 3 => 4 }
    MyHash[h].should == h
    MyHash[h].should == MyHash[h]
    h.should == MyHash[h]
  end
  
  specify "[] should return the value for key" do
    obj = Object.new
    h = { 1 => 2, 3 => 4, "foo" => "bar", obj => obj }
    h[1].should == 2
    h[3].should == 4
    h["foo"].should == "bar"
    h[obj].should == obj
  end
  
  specify "[] should return the default (immediate) value for missing keys" do
    h = Hash.new(5)
    h[:a].should == 5
    h[:a] = 0
    h[:a].should == 0
    h[:b].should == 5
    
    # The default default is nil
    { 0 => 0 }[5].should == nil
  end

  specify "[] shouldn't create copies of the immediate default value" do
    str = "foo"
    h = Hash.new(str)
    a = h[:a]
    b = h[:b]
    a << "bar"

    a.equal?(b).should == true
    a.should == "foobar"
    b.should == "foobar"
  end

  specify "[] should return the default (dynamic) value for missing keys" do
    h = Hash.new { |hash, k| k.kind_of?(Numeric) ? hash[k] = k + 2 : hash[k] = k }
    h[1].should == 3
    h['this'].should == 'this'
    h.should == {1 => 3, 'this' => 'this'}
    
    i = 0
    h = Hash.new { |hash, key| i += 1 }
    h[:foo].should == 1
    h[:foo].should == 2
    h[:bar].should == 3
  end

  specify "[] shouldn't return default values for keys with nil values" do
    h = Hash.new(5)
    h[:a] = nil
    h[:a].should == nil
    
    h = Hash.new() { 5 }
    h[:a] = nil
    h[:a].should == nil
  end
  
  specify "[] should compare keys with eql? semantics" do
    { 1.0 => "x" }[1].should == nil
    { 1.0 => "x" }[1.0].should == "x"
    { 1 => "x" }[1.0].should == nil
    { 1 => "x" }[1].should == "x"
  end

  specify "[] should compare key via hash" do
    # Can't use should_receive because it uses hash internally
    x = Object.new
    def x.hash() freeze; 0 end

    { }[x].should == nil
    x.frozen?.should == true
  end
    
  specify "[] shouldn't compare key with unknown hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = Object.new
    y = Object.new
    def x.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() freeze; 0 end
    def y.hash() 1 end

    { y => 1 }[x].should == nil
    x.frozen?.should == true
  end    
    
  specify "[] should compare key with found hash code via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    y = Object.new
    def y.hash() 0 end

    x = Object.new
    def x.hash()
      def self.eql?(o) taint; false; end
      return 0
    end
    
    { y => 1 }[x].should == nil
    x.tainted?.should == true
    
    x = Object.new
    def x.hash()
      def self.eql?(o) taint; true; end
      return 0
    end
    
    { y => 1 }[x].should == 1
    x.tainted?.should == true
  end  
  
  specify "[]= should associate the key with the value and return the value" do
    h = { :a => 1 }
    (h[:b] = 2).should == 2
    h.should == {:b=>2, :a=>1}
  end
  
  specify "[]= should duplicate and freeze string keys" do
    key = "foo"
    h = {}
    h[key] = 0
    key << "bar"

    h.should == { "foo" => 0 }
    h.keys[0].frozen?.should == true
  end

  specify "[]= should duplicate string keys using dup semantics" do
    # dup doesn't copy singleton methods
    key = "foo"
    def key.reverse() "bar" end
    h = {}
    h[key] = 0

    h.keys[0].reverse.should == "oof"
  end
  
  specify "clear should remove all key, value pairs" do
    h = { 1 => 2, 3 => 4 }
    h.clear.equal?(h).should == true
    h.should == {}
  end

  specify "clear shouldn't remove default values and procs" do
    h = Hash.new(5)
    h.clear
    h.default.should == 5

    h = Hash.new { 5 }
    h.clear
    h.default_proc.should_not == nil
  end

  specify "default should return the default value" do
    h = Hash.new(5)
    h.default.should == 5 
    {}.default.should == nil
  end
  
  specify "default= should set the default value" do
    h = Hash.new
    h.default = 99
    h.default.should == 99
  end

  specify "default= should unset the default proc" do
    h = Hash.new { 5 }
    h.default_proc.should_not == nil
    h.default = 99
    h.default_proc.should == nil
    
    h = Hash.new { 5 }
    h.default_proc.should_not == nil
    h.default = nil
    h.default_proc.should == nil
  end
  
  specify "default_proc should return the block passed to Hash.new" do
    h = Hash.new { |i| 'Paris' }
    p = h.default_proc
    p.call(1).should == 'Paris'
  end
  
  specify "default_proc should return nil if no block was passed to proc" do
    Hash.new.default_proc.should == nil
  end
  
  specify "delete should delete one entry whose key is == key and return the deleted value" do
    h = {:a => 5, :b => 2}
    h.delete(:b).should == 2
    h.should == {:a => 5}

    h = Hash.new
    k1 = ["x"]
    k2 = ["y"]
    # So they end up in the same bucket
    def k1.hash() 0 end
    def k2.hash() 0 end

    h[k1] = 1
    h[k2] = 2
    k1.replace(k2)
    # MRI deletes the one last inserted, but we won't spec that
    h.delete(k2)
    h.size.should == 1
  end
  
  specify "delete should return nil if the key is not found" do
    {:a => 1, :b => 10, :c => 100 }.delete(:d).should == nil
    Hash.new(:default).delete(:d).should == nil
    Hash.new() { :defualt }.delete(:d).should == nil
  end
  
  specify "delete_if should remove every entry for which block is true and returns self" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}
    h.delete_if { |k,v| v % 2 == 1 }.should == {:b => 2, :d => 4} 
    h.should == {:b => 2, :d => 4}
  end
  
  specify "each should call block once for each entry, passing key, value" do
    h = {}
    {:a, 1, :b, 2, :c, 3, :d, 5}.each { |k,v| h[k.to_s] = v.to_s }
    h.should == {"a" => "1", "b" => "2", "c" => "3", "d" => "5" }
  end
  
  specify "each_key should call block once for each key, passing key" do
    h = {}
    {1 => -1, 2 => -2, 3 => -3, 4 => -4 }.each_key { |k| h[k] = k }
    h.should == { 1 => 1, 2 => 2, 3 => 3, 4 => 4 }
  end
  
  specify "each_pair should be a synonym for each" do
    a, b = [], []
    {:a, 1, :b, 2, :c, 3, :d, 5}.each_pair { |k,v| a << "#{k} => #{v}" }
    {:a, 1, :b, 2, :c, 3, :d, 5}.each { |k,v| b << "#{k} => #{v}" }

    a.should == b
  end
  
  specify "each_value should should call block once for each key, passing value" do
    h = {}
    { :a => -5, :b => -3, :c => -2, :d => -1 }.each_value { |v| h[v.abs] = v }
    h.should == { 5 => -5, 1 => -1, 2 => -2, 3 => -3 }
  end
  
  specify "empty? should return true if the hash has no entries" do
    {}.empty?.should == true
    {1 => 1}.empty?.should == false
    Hash.new(5).empty?.should == true
    Hash.new { 5 }.empty?.should == true
  end
  
  specify "fetch should return the value for key" do
    { :a => 1, :b => -1 }.fetch(:b).should == -1
  end
  
  specify "fetch should raise IndexError if key is not found" do
    should_raise(IndexError) { {}.fetch(:a) }
  end
  
  specify "fetch with default should return default if key is not found" do
    {}.fetch(:a, 'not here!').should == "not here!"
  end
  
  specify "fetch with block should return value of block if key is not found" do
    {}.fetch('a') { |k| k + '!' }.should == "a!"
  end
  
  specify "has_key? should be a synonym for key?" do
    h = {:a => 1, :b => 2, :c => 3}
    h.has_key?(:a).should == h.key?(:a)
    h.has_key?(:b).should == h.key?(:b) 
    h.has_key?('b').should == h.key?('b') 
    h.has_key?(2).should == h.key?(2)
  end
  
  specify "has_value? should be a synonym for value?" do
    {:a => :b}.has_value?(:a).should == {:a => :b}.value?(:a)
    {1 => 2}.has_value?(2).should == {1 => 2}.value?(2)
  end
  
  specify "include? should be a synonym for key?" do
    h = {:a => 1, :b => 2, :c => 3}
    h.include?(:a).should   == h.key?(:a) 
    h.include?(:b).should   == h.key?(:b) 
    h.include?('b').should  == h.key?('b')
    h.include?(2).should    == h.key?(2)
  end

  specify "index should return the corresponding key for value" do
    {2 => 'a', 1 => 'b'}.index('b').should == 1
  end
  
  specify "index should return nil if the value is not found" do
    {:a => -1, :b => 3.14, :c => 2.718}.index(1).should == nil
  end
  
  specify "indexes and indices should be DEPRECATED synonyms for values_at" do
    h = {:a => 9, :b => 'a', :c => -10, :d => nil}
    h.indexes(:a, :d, :b).should == h.values_at(:a, :d, :b)
    h.indices(:a, :d, :b).should == h.values_at(:a, :d, :b)
  end
  
  specify "initialize_copy should be a synonym for replace" do
    Hash.new.send(:initialize_copy, { :a => 1, :b => 2 }).should == Hash.new.send(:replace, { :a => 1, :b => 2 })
  end
  
  specify "inspect should return a string representation of some kind" do
    {:a => 0, :b => -2, :c => 4, :d => -6}.inspect.class.should == String
  end
  
  specify "invert should return a new hash where keys are values and vice versa" do
    { 1 => 'a', 2 => 'b', 3 => 'c' }.invert.should == { 'a' => 1, 'b' => 2, 'c' => 3 }
  end
  
  specify "key? should return true if argument is a key" do
    h = { :a => 1, :b => 2, :c => 3 }
    h.key?(:a).should == true
    h.key?(:b).should == true
    h.key?('b').should == false
    h.key?(2).should == false
  end
  
  specify "key? should return false if the key was not found" do
    { 5 => 7 }.key?(7).should == false
  end

  specify "key? should return true if the key's matching value was nil" do
    { :xyz => nil }.key?(:xyz).should == true
  end

  specify "key? should return true if the key's matching value was false" do
    { :xyz => false }.key?(:xyz).should == true
  end

  specify "key? should return true if the key was found" do
    { :xyz => 9 }.key?(:xyz).should == true
  end

  specify "keys should return an array populated with keys" do
    { 1 => 2, 2 => 4, 4 => 8 }.keys.should == [1, 2, 4]
  end

  specify "length should return the number of entries" do
    {:a => 1, :b => 'c'}.length.should == 2
    {}.length.should == 0
  end
  
  specify "member? should be a synonym for key?" do
    h = {:a => 1, :b => 2, :c => 3}
    h.member?(:a).should == h.key?(:a)
    h.member?(:b).should == h.key?(:b)
    h.member?('b').should == h.key?('b')
    h.member?(2).should == h.key?(2)
  end
  
  specify "merge should return a new hash by combining self with the contents of other" do
    { 1, :a, 2, :b, 3, :c }.merge(:a => 1, :c => 2).should == { :c=> 2, 1 => :a, 2 => :b, :a => 1, 3 => :c }
  end
  
  specify "merge with block sets any duplicate key to the value of block" do
    { :a => 2, :b => 1 }.merge(:a => -2, :c => -3) { |k,v| -9 }.should == { :c => -3, :b => 1, :a => -9 }
  end
  
  specify "merge! should adds the entries from other, overwriting duplicate keys. Returns self" do
    h = { :_1 => 'a', :_2 => '3' }
    h.merge!(:_1 => '9', :_9 => 2).should == { :_1 => "9", :_2 => "3", :_9 => 2 }
    h.should == {:_1 => "9", :_2 => "3", :_9 => 2}
  end
  
  specify "merge! with block sets any duplicate key to the value of block" do
    h = { :a => 2, :b => -1 }
    h.merge!(:a => -2, :c => 1) { |k,v| 3.14 }.should == {:c => 1, :b => -1, :a => 3.14}
    h.should == {:c => 1, :b => -1, :a => 3.14}
  end
  
  specify "rehash should be provided" do
    Hash.new.respond_to?(:rehash).should == true
  end
  
  specify "reject should be equivalent to hsh.dup.delete_if" do
    h = { :a => 'a', :b => 'b', :c => 'd' }
    h.reject { |k,v| k == 'd' }.should == (h.dup.delete_if { |k, v| k == 'd' })
  end
  
  specify "reject! is equivalent to delete_if if changes are made" do
    {:a => 2}.reject! { |k,v| v > 1 }.should == ({:a => 2}.delete_if { |k, v| v > 1 })
  end
  
  specify "reject! should return nil if no changes were made" do
    { :a => 1 }.reject! { |k,v| v > 1 }.should == nil
  end
  
  specify "replace should replace the contents of self with other" do
    h = { :a => 1, :b => 2 }
    h.replace(:c => -1, :d => -2).should == { :c => -1, :d => -2 }
  end
  
  specify "select should return an array of entries for which block is true" do
    a = { :a => 9, :c => 4, :b => 5, :d => 2 }.select { |k,v| v % 2 == 0 }
    a.sort { |a,b| a.to_s <=> b.to_s }.should == [[:c, 4], [:d, 2]]
  end
  
  specify "shift should remove an entry from hash and return it in a two-element array" do
    h = { :a => 2 }
    h.shift.should == [:a, 2]
    h.should == {}
  end
  
  specify "size should be a synonym for length" do
    { :a => 1, :b => 'c' }.size.should == {:a => 1, :b => 'c'}.length 
    {}.size.should == {}.length
  end
  
  specify "sort should convert self to a nested array of [key, value] arrays and sort with Array#sort" do
    { 'a' => 'b', '1' => '2', 'b' => 'a' }.sort.should == [["1", "2"], ["a", "b"], ["b", "a"]]
  
  specify "sort should work when some of the keys are themselves arrays" do
    { [1,2] => 5, [1,1] => 5 }.sort.should == [[[1,1],5], [[1,2],5]]
  end
  
  end
  specify "sort with block should use block to sort array" do
    { 1 => 2, 2 => 9, 3 => 4 }.sort { |a,b| b <=> a }.should == [[3, 4], [2, 9], [1, 2]]
  end

  specify "store should be a synonym for []=" do
    h1, h2 = {:a => 1}, {:a => 1}
    h1.store(:b, 2).should == (h2[:b] = 2)
    h1.should == h2
  end
  
  specify "to_a should return a nested array of [key, value] arrays" do
    {:a => 1, 1 => :a, 3 => :b, :b => 5}.to_a.sort { |a,b| a.to_s <=> b.to_s }.should == [[1, :a], [3, :b], [:a, 1], [:b, 5]]
  end
  
  specify "to_hash should should return self" do
    h = {}
    h.to_hash.equal?(h).should == true
  end
  
  specify "to_s should return a string by calling Hash#to_a and using Array#join with default separator" do
    { :fun => 'fun', 'fun' => :fun }.to_s.should == 'funfunfunfun'
  end
  
  specify "update should be a synonym for merge!" do
    h1 = { :_1 => 'a', :_2 => '3' }
    h2 = h1.dup

    h1.update(:_1 => '9', :_9 => 2).should == h2.merge!(:_1 => '9', :_9 => 2)
    h1.should == h2
  end
  
  specify "update with block should be a synonym for merge!" do
    h1 = { :a => 2, :b => -1 }
    h2 = h1.dup

    h1.update(:a => -2, :c => 1) { |k,v| 3.14 }.should == h2.update(:a => -2, :c => 1) { |k,v| 3.14 }
    h1.should == h2
  end
  
  specify "value? returns true if the value exists in the hash" do
    {:a => :b}.value?(:a).should == false
    {1 => 2}.value?(2).should == true
  end
  
  specify "values should return an array of values" do
    { 1 => :a, 'a' => :a, 'the' => 'lang'}.values.sort {|a, b| a.to_s <=> b.to_s}.should == [:a, :a, 'lang']
  end
  
  specify "values_at should return an array of values for the given keys" do
    {:a => 9, :b => 'a', :c => -10, :d => nil}.values_at(:a, :d, :b).should == [9, nil, 'a']
  end
end
