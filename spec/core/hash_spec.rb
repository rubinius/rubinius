require File.dirname(__FILE__) + '/../spec_helper'

# ==, [], []=, clear, default, default=, default_proc, delete,
# delete_if, each, each_key, each_pair, each_value, empty?, fetch,
# has_key?, has_value?, include?, index, indexes, indices,
# initialize_copy, inspect, invert, key?, keys, length, member?,
# merge, merge!, rehash, reject, reject!, replace, select, shift,
# size, sort, store, to_a, to_hash, to_s, update, value?, values,
# values_at

class MyHash < Hash; end
class ToHashHash < Hash
  def to_hash() { "to_hash" => "was", "called!" => "duh." } end
end

empty = {}
empty.freeze

hash = {1 => 2, 3 => 4}
hash.freeze

describe "Hash" do
  it "includes Enumerable" do
    Hash.include?(Enumerable).should == true
  end
end

describe "Hash.[]" do
  it "creates a Hash; values can be provided as the argument list" do
    Hash[:a, 1, :b, 2].should == {:a => 1, :b => 2}
    Hash[].should == {}
  end

  it "creates a Hash; values can be provided as one single hash" do
    Hash[:a => 1, :b => 2].should == {:a => 1, :b => 2} 
    Hash[{1 => 2, 3 => 4}].should == {1 => 2, 3 => 4}
    Hash[{}].should == {}
  end

  it "raises on odd parameter list count" do
    should_raise(ArgumentError) { Hash[1, 2, 3] }
  end
 
  it "raises when mixing argument styles" do
    should_raise(ArgumentError) { Hash[1, 2, {3 => 4}] }
    Hash[1, 2, 3, {3 => 4}].should == {1 => 2, 3 => {3 => 4}}
  end
  
  it "does not call to_hash" do
    obj = Object.new
    def obj.to_hash() { 1 => 2, 3 => 4 } end
    should_raise(ArgumentError) { Hash[obj] }
  end

  it "returns an instance of the class it's called on" do
    Hash[MyHash[1, 2]].class.should == Hash
    MyHash[Hash[1, 2]].class.should == MyHash
  end
end

describe "Hash.new" do
  it "creates a new Hash with default object if pass a default argument " do
    Hash.new(5).default.should == 5
    Hash.new({}).default.should == {}
  end

  it "does not create a copy of the default argument" do
    str = "foo"
    Hash.new(str).default.equal?(str).should == true
  end
  
  it "creates a Hash with a default_proc if passed a block" do
    Hash.new.default_proc.should == nil
    
    h = Hash.new { |x| "Answer to #{x}" }
    h.default_proc.call(5).should == "Answer to 5"
    h.default_proc.call("x").should == "Answer to x"
  end
  
  it "raises ArgumentError if passed both default argument and default block" do
    should_raise(ArgumentError) { Hash.new(5) { 0 } }
    should_raise(ArgumentError) { Hash.new(nil) { 0 } }
  end
end

describe "Hash#==" do
  it "returns true if other Hash has the same number of keys and each key-value pair matches" do
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
  
  it "calls to_hash on its argument" do
    obj = Object.new
    obj.should_receive(:to_hash, :returning => {1 => 2, 3 => 4})
    
    {1 => 2, 3 => 4}.should == obj
  end
  
  it "does not call to_hash on hash subclasses" do
    {5 => 6}.should == ToHashHash[5 => 6]
  end
  
  it "returns false when the numbers of keys differ without comparing any elements" do
    obj = Object.new
    obj.should_not_receive(:==)
    obj.should_not_receive(:eql?)

    {}.should_not == { obj => obj }
    { obj => obj }.should_not == {}
  end
  
  it "compares keys with eql? semantics" do
    { 1.0 => "x" }.should == { 1.0 => "x" }
    { 1 => "x" }.should_not == { 1.0 => "x" }
    { 1.0 => "x" }.should_not == { 1 => "x" }
  end

  it "first compares keys via hash" do
    # Can't use should_receive because it uses hash internally
    x = Object.new
    def x.hash() freeze; 0 end
    y = Object.new
    def y.hash() freeze; 0 end

    { x => 1 } == { y => 1 }
    x.frozen?.should == true
    y.frozen?.should == true
  end
    
  it "does not compare keys with different hash codes via eql?" do
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
    
  it "compares keys with matching hash codes via eql?" do
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
  
  it "compares values with == semantics" do
    { "x" => 1.0 }.should == { "x" => 1 }
  end
  
  it "does not compare values when keys don't match" do
    value = Object.new
    value.should_not_receive(:==)
    { 1 => value }.should_not == { 2 => value }
  end
  
  it "compares values when keys match" do
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

  it "ignores hash class differences" do
    h = { 1 => 2, 3 => 4 }
    MyHash[h].should == h
    MyHash[h].should == MyHash[h]
    h.should == MyHash[h]
  end  
end

describe "Hash#[]" do
  it "returns the value for key" do
    obj = Object.new
    h = { 1 => 2, 3 => 4, "foo" => "bar", obj => obj }
    h[1].should == 2
    h[3].should == 4
    h["foo"].should == "bar"
    h[obj].should == obj
  end

  it "returns the default (immediate) value for missing keys" do
    h = Hash.new(5)
    h[:a].should == 5
    h[:a] = 0
    h[:a].should == 0
    h[:b].should == 5

    # The default default is nil
    { 0 => 0 }[5].should == nil
  end

  it "does not create copies of the immediate default value" do
    str = "foo"
    h = Hash.new(str)
    a = h[:a]
    b = h[:b]
    a << "bar"

    a.equal?(b).should == true
    a.should == "foobar"
    b.should == "foobar"
  end

  it "returns the default (dynamic) value for missing keys" do
    h = Hash.new { |hsh, k| k.kind_of?(Numeric) ? hsh[k] = k + 2 : hsh[k] = k }
    h[1].should == 3
    h['this'].should == 'this'
    h.should == {1 => 3, 'this' => 'this'}

    i = 0
    h = Hash.new { |hsh, key| i += 1 }
    h[:foo].should == 1
    h[:foo].should == 2
    h[:bar].should == 3
  end

  it "does not return default values for keys with nil values" do
    h = Hash.new(5)
    h[:a] = nil
    h[:a].should == nil

    h = Hash.new() { 5 }
    h[:a] = nil
    h[:a].should == nil
  end

  it "compares keys with eql? semantics" do
    { 1.0 => "x" }[1].should == nil
    { 1.0 => "x" }[1.0].should == "x"
    { 1 => "x" }[1.0].should == nil
    { 1 => "x" }[1].should == "x"
  end

  it "compares key via hash" do
    # Can't use should_receive because it uses hash internally
    x = Object.new
    def x.hash() freeze; 0 end

    { }[x].should == nil
    x.frozen?.should == true
  end

  # True for all other methods using hash codes as well...
  it "calls % on hash code from hash()" do
    x = Object.new
    def x.hash() @hash end
    hc = Object.new
    x.instance_variable_set(:@hash, hc)

    hc.should_receive(:%, :returning => 0)
    h = {1 => 2}
    h[x].should == nil
  end

  it "does not compare key with unknown hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = Object.new
    y = Object.new
    def x.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() freeze; 0 end
    def y.hash() 1 end

    { y => 1 }[x].should == nil
    x.frozen?.should == true
  end    

  it "compares key with found hash code via eql?" do
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
end

hash_store = shared "Hash#store" do |cmd|
  describe "Hash##{cmd}" do
    it "associates the key with the value and return the value" do
      h = { :a => 1 }
      h.send(cmd, :b, 2).should == 2
      h.should == {:b=>2, :a=>1}
    end

    it "duplicates and freezes string keys" do
      key = "foo"
      h = {}
      h.send(cmd, key, 0)
      key << "bar"

      h.should == { "foo" => 0 }
      h.keys[0].frozen?.should == true
    end

    it "duplicates string keys using dup semantics" do
      # dup doesn't copy singleton methods
      key = "foo"
      def key.reverse() "bar" end
      h = {}
      h.send(cmd, key, 0)

      h.keys[0].reverse.should == "oof"
    end  

    it "raises TypeError if called on a frozen instance" do
      should_raise(TypeError) { hash.send(cmd, 1, 2) }
    end
  end
end

describe "Hash#[]=" do
  it_behaves_like(hash_store, :[]=)
end

describe "Hash#clear" do
  it "removes all key, value pairs" do
    h = { 1 => 2, 3 => 4 }
    h.clear.equal?(h).should == true
    h.should == {}
  end

  it "does not remove default values and procs" do
    h = Hash.new(5)
    h.clear
    h.default.should == 5

    h = Hash.new { 5 }
    h.clear
    h.default_proc.should_not == nil
  end

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.clear }
    should_raise(TypeError) { empty.clear }
  end
end

describe "Hash#default" do
  it "returns the default value" do
    h = Hash.new(5)
    h.default.should == 5
    h.default(4).should == 5
    {}.default.should == nil
    {}.default(4).should == nil
  end

  it "uses the default proc to compute a default value, passing given key" do
    h = Hash.new { |*args| args }
    h.default(nil).should == [h, nil]
    h.default(5).should == [h, 5]
  end
  
  it "calls default proc with nil arg if passed a default proc but no arg" do
    h = Hash.new { |*args| args }
    h.default.should == [h, nil]
  end
  
end

describe "Hash#default=" do
  it "sets the default value" do
    h = Hash.new
    h.default = 99
    h.default.should == 99
  end

  it "unsets the default proc" do
    [99, nil, lambda { 6 }].each do |default|
      h = Hash.new { 5 }
      h.default_proc.should_not == nil
      h.default = default
      h.default.should == default
      h.default_proc.should == nil
    end
  end

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.default = nil }
  end
end

describe "Hash#default_proc" do
  it "returns the block passed to Hash.new" do
    h = Hash.new { |i| 'Paris' }
    p = h.default_proc
    p.call(1).should == 'Paris'
  end
  
  it "returns nil if no block was passed to proc" do
    Hash.new.default_proc.should == nil
  end
end

describe "Hash#delete" do
  it "removes the first entry (#keys order) whose key is == key and return the deleted value" do
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
    
    first_value = h.values.first
    h.delete(k2).should == first_value
    h.size.should == 1
  end

  it "calls supplied block if the key is not found" do
    {:a => 1, :b => 10, :c => 100 }.delete(:d) { 5 }.should == 5
    Hash.new(:default).delete(:d) { 5 }.should == 5
    Hash.new() { :defualt }.delete(:d) { 5 }.should == 5
  end
  
  it "returns nil if the key is not found when no block is given" do
    {:a => 1, :b => 10, :c => 100 }.delete(:d).should == nil
    Hash.new(:default).delete(:d).should == nil
    Hash.new() { :defualt }.delete(:d).should == nil
  end

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.delete("foo") }
    should_raise(TypeError) { empty.delete("foo") }
  end
end

hash_iteration_method = shared "Iteration method" do |cmd|
  # These are the only ones that actually have the exceptions on MRI 1.8.
  # sort and reject don't raise!
  # 
  #   delete_if each each_key each_pair each_value merge merge! reject!
  #   select update
  #
  describe "Hash##{cmd}" do
    hsh = {1 => 2, 3 => 4, 5 => 6}  
    big_hash = {}
    100.times { |k| big_hash[k.to_s] = k }    
       
    it "raise RuntimeError if #rehash is called from block" do
      h = hsh.dup
      args = cmd.to_s[/merge|update/] ? [h] : []
      
      should_raise(RuntimeError, "rehash occurred during iteration") do
        h.send(cmd, *args) { h.rehash }
      end
    end

    it "raises if lots of new entries are added from block" do
      h = hsh.dup
      args = cmd.to_s[/merge|update/] ? [h] : []

      should_raise(RuntimeError, "hash modified during iteration") do
        h.send(cmd, *args) { |*x| h.merge!(big_hash) }
      end
    end

    it "does not affect yielded items by removing the current element" do
      n = 3
      
      h = Array.new(n) { hsh.dup }
      args = Array.new(n) { |i| cmd.to_s[/merge|update/] ? [h[i]] : [] }
      r = Array.new(n) { [] }
      
      h[0].send(cmd, *args[0]) { |*x| r[0] << x; true }
      h[1].send(cmd, *args[1]) { |*x| r[1] << x; h[1].shift; true }
      h[2].send(cmd, *args[2]) { |*x| r[2] << x; h[2].delete(h[2].keys.first); true }
      
      r[1..-1].each do |yielded|
        yielded.should == r[0]
      end
    end
  end
end

describe "Hash#delete_if" do
  it "yields two arguments: key and value" do
    all_args = []
    {1 => 2, 3 => 4}.delete_if { |*args| all_args << args }
    all_args.should == [[1, 2], [3, 4]]
  end
  
  it "removes every entry for which block is true and returns self" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}
    h.delete_if { |k,v| v % 2 == 1 }.equal?(h).should == true
    h.should == {:b => 2, :d => 4}
  end
  
  it "processes entries with the same order as each()" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}

    each_pairs = []
    delete_pairs = []
    h.each { |pair| each_pairs << pair }
    h.delete_if { |*pair| delete_pairs << pair }

    each_pairs.should == delete_pairs
  end

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.delete_if { false } }
    should_raise(TypeError) { empty.delete_if { true } }
  end
  
  it_behaves_like(hash_iteration_method, :delete_if)
end

describe "Hash#each" do
  it "yields one argument: [key, value]" do
    all_args = []
    {1 => 2, 3 => 4}.each { |*args| all_args << args }
    all_args.should == [[[1, 2]], [[3, 4]]]
  end
  
  it "calls block once for each entry, passing key, value" do
    r = {}
    h = {:a => 1, :b => 2, :c => 3, :d => 5}
    h.each { |k,v| r[k.to_s] = v.to_s }.equal?(h).should == true
    r.should == {"a" => "1", "b" => "2", "c" => "3", "d" => "5" }
  end

  it "uses the same order as keys() and values()" do
    h = {:a => 1, :b => 2, :c => 3, :d => 5}
    keys = []
    values = []

    h.each do |k, v|
      keys << k
      values << v
    end
    
    keys.should == h.keys
    values.should == h.values
  end

  it_behaves_like(hash_iteration_method, :each)
end

describe "Hash#each_key" do
  it "calls block once for each key, passing key" do
    r = {}
    h = {1 => -1, 2 => -2, 3 => -3, 4 => -4 }
    h.each_key { |k| r[k] = k }.equal?(h).should == true
    r.should == { 1 => 1, 2 => 2, 3 => 3, 4 => 4 }
  end

  it "processes keys in the same order as keys()" do
    keys = []
    h = {1 => -1, 2 => -2, 3 => -3, 4 => -4 }
    h.each_key { |k| keys << k }
    keys.should == h.keys
  end

  it_behaves_like(hash_iteration_method, :each_key)
end

describe "Hash#each_pair" do
  it "processes all pairs, yielding two arguments: key and value" do
    all_args = []

    h = {1 => 2, 3 => 4}
    h.each_pair { |*args| all_args << args }.equal?(h).should == true

    all_args.should == [[1, 2], [3, 4]]
  end

  it_behaves_like(hash_iteration_method, :each_pair)
end

describe "Hash#each_value" do
  it "calls block once for each key, passing value" do
    r = []
    h = { :a => -5, :b => -3, :c => -2, :d => -1, :e => -1 }
    h.each_value { |v| r << v }.equal?(h).should == true
    r.sort.should == [-5, -3, -2, -1, -1]
  end

  it "processes values in the same order as values()" do
    values = []
    h = { :a => -5, :b => -3, :c => -2, :d => -1, :e => -1 }
    h.each_value { |v| values << v }
    values.should == h.values
  end

  it_behaves_like(hash_iteration_method, :each_value)
end

describe "Hash#empty?" do
  it "returns true if the hash has no entries" do
    {}.empty?.should == true
    {1 => 1}.empty?.should == false
    Hash.new(5).empty?.should == true
    Hash.new { 5 }.empty?.should == true
  end
end

describe "Hash#fetch" do
  it "returns the value for key" do
    { :a => 1, :b => -1 }.fetch(:b).should == -1
  end
  
  it "raises IndexError if key is not found" do
    should_raise(IndexError) { {}.fetch(:a) }
    should_raise(IndexError) { Hash.new(5).fetch(:a) }
    should_raise(IndexError) { Hash.new { 5 }.fetch(:a) }
  end
  
  it "returns default if key is not found when passed a default" do
    {}.fetch(:a, nil).should == nil
    {}.fetch(:a, 'not here!').should == "not here!"
    { :a => nil }.fetch(:a, 'not here!').should == nil
  end
  
  it "returns value of block if key is not found when passed a block" do
    {}.fetch('a') { |k| k + '!' }.should == "a!"
  end

  it "gives precedence to the default block over the default argument when passed both" do
    {}.fetch(9, :foo) { |i| i * i }.should == 81
  end
end

hash_key_p = shared "Hash#key?" do |cmd|
  describe "Hash##{cmd}" do
    it "returns true if argument is a key" do
      h = { :a => 1, :b => 2, :c => 3, 4 => 0 }
      h.send(cmd, :a).should == true
      h.send(cmd, :b).should == true
      h.send(cmd, 'b').should == false
      h.send(cmd, 2).should == false
      h.send(cmd, 4).should == true
      h.send(cmd, 4.0).should == false
    end

    it "returns true if the key's matching value was nil" do
      { :xyz => nil }.send(cmd, :xyz).should == true
    end

    it "returns true if the key's matching value was false" do
      { :xyz => false }.send(cmd, :xyz).should == true
    end
  end
end

describe "Hash#has_key?" do
  it_behaves_like(hash_key_p, :has_key?)
end

describe "Hash#include?" do
  it_behaves_like(hash_key_p, :include?)
end

hash_value_p = shared "Hash#value?" do |cmd|
  describe "Hash##{cmd}" do
    it "returns true if the value exists in the hash" do
      {:a => :b}.send(cmd, :a).should == false
      {1 => 2}.send(cmd, 2).should == true
      h = Hash.new(5)
      h.send(cmd, 5).should == false
      h = Hash.new { 5 }
      h.send(cmd, 5).should == false
    end

    it "uses == semantics for comparing values" do
      { 5 => 2.0 }.send(cmd, 2).should == true
    end
  end
end

describe "Hash#has_value?" do
  it_behaves_like(hash_value_p, :has_value?)
end

describe "Hash#index" do
  it "returns the corresponding key for value" do
    {2 => 'a', 1 => 'b'}.index('b').should == 1
  end
  
  it "returns nil if the value is not found" do
    {:a => -1, :b => 3.14, :c => 2.718}.index(1).should == nil
    Hash.new(5).index(5).should == nil
  end

  it "compares values using ==" do
    {1 => 0}.index(0.0).should == 1
    {1 => 0.0}.index(0).should == 1
    
    needle = Object.new
    inhash = Object.new
    inhash.should_receive(:==, :with => [needle], :returning => true)
    
    {1 => inhash}.index(needle).should == 1
  end

  it "compares values with same order as keys() and values()" do
    h = {1 => 0, 2 => 0, 3 => 0, 4 => 0, 5 => 0, 6 => 0}
    h.index(0).should == h.keys.first
    
    needle = Object.new
    h = {1 => Object.new, 3 => Object.new, 4 => Object.new, 42 => Object.new }
    h.values[0].should_receive(:==, :with => [needle], :returning => false)
    h.values[1].should_receive(:==, :with => [needle], :returning => false)
    h.values[2].should_receive(:==, :with => [needle], :returning => true)
    h.values[3].should_not_receive(:==)
    
    h.index(needle).should == h.keys[2]
  end
end

describe "Hash#indexes" do
  it "is a DEPRECATED synonyms for values_at" do
    h = {:a => 9, :b => 'a', :c => -10, :d => nil}
    h.indexes(:a, :d, :b).should == h.values_at(:a, :d, :b)
    h.indexes().should == h.values_at()
  end
end

describe "Hash#indices" do
  it "is a DEPRECATED synonyms for values_at" do
    h = {:a => 9, :b => 'a', :c => -10, :d => nil}
    h.indices(:a, :d, :b).should == h.values_at(:a, :d, :b)
    h.indices().should == h.values_at()
  end
end

describe "Hash#initialize" do
  it "is private" do
    {}.private_methods.map { |m| m.to_s }.include?("initialize").should == true
  end

  it "can be used to reset default_proc" do
    h = { "foo" => 1, "bar" => 2 }
    h.default_proc.should == nil
    h.instance_eval { initialize { |h, k| k * 2 } }
    h.default_proc.should_not == nil
    h["a"].should == "aa"
  end

  it "raises TypeError if called on a frozen instance" do
    hash.instance_eval do
      should_raise(TypeError) { initialize() }
      should_raise(TypeError) { initialize(nil) }
      should_raise(TypeError) { initialize(5) }
      should_raise(TypeError) { initialize { 5 } }
    end
  end
end

hash_replace = shared "Hash#replace" do |cmd|
  describe "Hash##{cmd}" do
    it "replaces the contents of self with other" do
      h = { :a => 1, :b => 2 }
      h.send(cmd, :c => -1, :d => -2).equal?(h).should == true
      h.should == { :c => -1, :d => -2 }
    end

    it "calls to_hash on its argument" do
      obj = Object.new
      obj.should_receive(:to_hash, :returning => {1 => 2, 3 => 4})

      h = {}
      h.send(cmd, obj)
      h.should == {1 => 2, 3 => 4}

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_hash], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_hash], :returning => {})

      h.send(cmd, obj)
      h.should == {}
    end

    it "calls to_hash on hash subclasses" do
      h = {}
      h.send(cmd, ToHashHash[1 => 2])
      h.should == {1 => 2}
    end

    it "does not transfer default values" do
      hash_a = {}
      hash_b = Hash.new(5)
      hash_a.send(cmd, hash_b)
      hash_a.default.should == 5

      hash_a = {}
      hash_b = Hash.new { |h, k| k * 2 }
      hash_a.send(cmd, hash_b)
      hash_a.default(5).should == 10

      hash_a = Hash.new { |h, k| k * 5 }
      hash_b = Hash.new(lambda { raise "Should not invoke lambda" })
      hash_a.send(cmd, hash_b)
      hash_a.default.should == hash_b.default
    end

    it "raises TypeError if called on a frozen instance" do
      hash.send(cmd, hash) # ok, nothing changed
      should_raise(TypeError) { hash.send(cmd, empty) }
    end
  end
end

describe "Hash#initialize_copy" do
  it "is private" do
    {}.private_methods.map { |m| m.to_s }.include?("initialize_copy").should == true
  end

  it_behaves_like(hash_replace, :initialize_copy)
end

describe "Hash#inspect" do
  it "returns a string representation with same order as each()" do
    h = {:a => [1, 2], :b => -2, :d => -6, nil => nil}
    
    pairs = []
    h.each do |key, value|
      pairs << key.inspect + "=>" + value.inspect
    end
    
    str = '{' + pairs.join(', ') + '}'
    h.inspect.should == str
  end

  it "calls inspect on keys and values" do
    key = Object.new
    val = Object.new
    key.should_receive(:inspect, :returning => 'key')
    val.should_receive(:inspect, :returning => 'val')
    
    { key => val }.inspect.should == '{key=>val}'
  end

  it "handles recursive hashes" do
    x = {}
    x[0] = x
    x.inspect.should == '{0=>{...}}'

    x = {}
    x[x] = 0
    x.inspect.should == '{{...}=>0}'

    x = {}
    x[x] = x
    x.inspect.should == '{{...}=>{...}}'

    x = {}
    y = {}
    x[0] = y
    y[1] = x
    x.inspect.should == "{0=>{1=>{...}}}"
    y.inspect.should == "{1=>{0=>{...}}}"

    x = {}
    y = {}
    x[y] = 0
    y[x] = 1
    x.inspect.should == "{{{...}=>1}=>0}"
    y.inspect.should == "{{{...}=>0}=>1}"
    
    x = {}
    y = {}
    x[y] = x
    y[x] = y
    x.inspect.should == "{{{...}=>{...}}=>{...}}"
    y.inspect.should == "{{{...}=>{...}}=>{...}}"
  end
end

describe "Hash#invert" do
  it "returns a new hash where keys are values and vice versa" do
    { 1 => 'a', 2 => 'b', 3 => 'c' }.invert.should == { 'a' => 1, 'b' => 2, 'c' => 3 }
  end
  
  it "handles collisions by overriding with the key coming later in keys()" do
    h = { :a => 1, :b => 1 }
    override_key = h.keys.last
    h.invert[1].should == override_key
  end

  it "compares new keys with eql? semantics" do
    h = { :a => 1.0, :b => 1 }
    i = h.invert
    i[1.0].should == :a
    i[1].should == :b
  end
  
  it "does not return subclass instances for subclasses" do
    MyHash[1 => 2, 3 => 4].invert.class.should == Hash
    MyHash[].invert.class.should == Hash
  end
end

describe "Hash#key?" do
  it_behaves_like(hash_key_p, :key?)
end

describe "Hash#keys" do
  it "returns an array populated with keys" do
    {}.keys.should == []
    {}.keys.class.should == Array
    Hash.new(5).keys.should == []
    Hash.new { 5 }.keys.should == []
    { 1 => 2, 2 => 4, 4 => 8 }.keys.should == [1, 2, 4]
    { 1 => 2, 2 => 4, 4 => 8 }.keys.class.should == Array
    { nil => nil }.keys.should == [nil]
  end

  it "it uses the same order as #values" do
    h = { 1 => "1", 2 => "2", 3 => "3", 4 => "4" }
    
    h.size.times do |i|
      h[h.keys[i]].should == h.values[i]
    end
  end
end

hash_length = shared "Hash#length" do |cmd|
  describe "Hash##{cmd}" do
    it "returns the number of entries" do
      {:a => 1, :b => 'c'}.send(cmd).should == 2
      {:a => 1, :b => 2, :a => 2}.send(cmd).should == 2
      {:a => 1, :b => 1, :c => 1}.send(cmd).should == 3
      {}.send(cmd).should == 0
      Hash.new(5).send(cmd).should == 0
      Hash.new { 5 }.send(cmd).should == 0
    end
  end
end

describe "Hash#length" do
  it_behaves_like(hash_length, :length)
end

describe "Hash#member?" do
  it_behaves_like(hash_key_p, :member?)
end

describe "Hash#merge" do
  it "returns a new hash by combining self with the contents of other" do
    { 1 => :a, 2 => :b, 3 => :c }.merge(:a => 1, :c => 2).should == { :c => 2, 1 => :a, 2 => :b, :a => 1, 3 => :c }
  end
  
  it "sets any duplicate key to the value of block if passed a block" do
    h1 = { :a => 2, :b => 1, :d => 5}
    h2 = { :a => -2, :b => 4, :c => -3 }
    r = h1.merge(h2) { |k,x,y| nil }
    r.should == { :a => nil, :b => nil, :c => -3, :d => 5 }
      
    r = h1.merge(h2) { |k,x,y| "#{k}:#{x+2*y}" }
    r.should == { :a => "a:-2", :b => "b:9", :c => -3, :d => 5 }

    should_raise(IndexError) do
      h1.merge(h2) { |k, x, y| raise(IndexError) }
    end

    r = h1.merge(h1) { |k,x,y| :x }
    r.should == { :a => :x, :b => :x, :d => :x }
  end
  
  it "calls to_hash on its argument" do
    obj = Object.new
    obj.should_receive(:to_hash, :returning => {1 => 2})
    {3 => 4}.merge(obj).should == {1 => 2, 3 => 4}
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_hash], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_hash], :returning => { 1 => 2})
    {3 => 4}.merge(obj).should == {1 => 2, 3 => 4}
  end

  it "does not call to_hash on hash subclasses" do    
    {3 => 4}.merge(ToHashHash[1 => 2]).should == {1 => 2, 3 => 4}
  end

  it "returns subclass instance for subclasses" do
    MyHash[1 => 2, 3 => 4].merge({1 => 2}).class.should == MyHash
    MyHash[].merge({1 => 2}).class.should == MyHash

    {1 => 2, 3 => 4}.merge(MyHash[1 => 2]).class.should == Hash
    {}.merge(MyHash[1 => 2]).class.should == Hash
  end
  
  it "processes entries with same order as each()" do
    h = {1 => 2, 3 => 4, 5 => 6, "x" => nil, nil => 5, [] => []}
    merge_pairs = []
    each_pairs = []
    h.each { |pair| each_pairs << pair }
    h.merge(h) { |k, v1, v2| merge_pairs << [k, v1] }
    merge_pairs.should == each_pairs
  end

  it_behaves_like(hash_iteration_method, :merge)
end

hash_update = shared "Hash#update" do |cmd|
  describe "Hash##{cmd}" do
    it "adds the entries from other, overwriting duplicate keys. Returns self" do
      h = { :_1 => 'a', :_2 => '3' }
      h.send(cmd, :_1 => '9', :_9 => 2).equal?(h).should == true
      h.should == {:_1 => "9", :_2 => "3", :_9 => 2}
    end

    it "sets any duplicate key to the value of block if passed a block" do
      h1 = { :a => 2, :b => -1 }
      h2 = { :a => -2, :c => 1 }
      h1.send(cmd, h2) { |k,x,y| 3.14 }.equal?(h1).should == true
      h1.should == {:c => 1, :b => -1, :a => 3.14}

      h1.send(cmd, h1) { nil }
      h1.should == { :a => nil, :b => nil, :c => nil }
    end

    it "calls to_hash on its argument" do
      obj = Object.new
      obj.should_receive(:to_hash, :returning => {1 => 2})
      {3 => 4}.send(cmd, obj).should == {1 => 2, 3 => 4}

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_hash], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_hash], :returning => { 1 => 2})
      {3 => 4}.send(cmd, obj).should == {1 => 2, 3 => 4}
    end

    it "does not call to_hash on hash subclasses" do    
      {3 => 4}.send(cmd, ToHashHash[1 => 2]).should == {1 => 2, 3 => 4}
    end

    it "processes entries with same order as merge()" do
      h = {1 => 2, 3 => 4, 5 => 6, "x" => nil, nil => 5, [] => []}
      merge_bang_pairs = []
      merge_pairs = []
      h.merge(h) { |*arg| merge_pairs << arg }
      h.send(cmd, h) { |*arg| merge_bang_pairs << arg }
      merge_bang_pairs.should == merge_pairs
    end

    it "raises TypeError if called on a frozen instance" do
      hash.send(cmd, empty) # ok, empty
      should_raise(TypeError) { hash.send(cmd, 1 => 2) }
    end
  end
end

describe "Hash#merge!" do
  it_behaves_like(hash_update, :merge!)

  it_behaves_like(hash_iteration_method, :merge!)
end

describe "Hash#rehash" do
  it "reorganizes the hash by recomputing all key hash codes" do
    k1 = [1]
    k2 = [2]
    h = {}
    h[k1] = 0
    h[k2] = 1

    k1 << 2
    h.key?(k1).should == false
    h.keys.include?(k1).should == true
    
    h.rehash.equal?(h).should == true
    h.key?(k1).should == true
    h[k1].should == 0
    
    k1 = Object.new
    k2 = Object.new
    v1 = Object.new
    v2 = Object.new
    
    # Can't use should_receive here because it uses hash() internally
    def v1.hash() raise("values shouldn't be rehashed"); end
    def v2.hash() raise("values shouldn't be rehashed"); end

    h = { k1 => v1, k2 => v2 }
    k1.frozen?.should == false
    k2.frozen?.should == false

    def k1.hash() freeze; 0 end
    def k2.hash() freeze; 0 end
    
    h.rehash
    k1.frozen?.should == true
    k2.frozen?.should == true    
  end
  
  it "gives precedence to keys coming later in keys() on collisions" do
    k1 = [1]
    k2 = [2]
    h = {}
    h[k1] = 0
    h[k2] = 1

    k1.replace(k2)
    override_val = h[h.keys.last]
    h.rehash
    h[k1].should == override_val
  end

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.rehash }
    should_raise(TypeError) { empty.rehash }
  end  
end

describe "Hash#reject" do
  it "should be equivalent to hsh.dup.delete_if" do
    h = { :a => 'a', :b => 'b', :c => 'd' }
    h.reject { |k,v| k == 'd' }.should == (h.dup.delete_if { |k, v| k == 'd' })
    
    all_args_reject = []
    all_args_delete_if = []
    h = {1 => 2, 3 => 4}
    h.reject { |*args| all_args_reject << args }
    h.delete_if { |*args| all_args_delete_if << args }
    all_args_reject.should == all_args_delete_if
    
    h = { 1 => 2 }
    # dup doesn't copy singleton methods
    def h.to_a() end
    h.reject { false }.to_a.should == [[1, 2]]
  end
  
  it "returns subclass instance for subclasses" do
    MyHash[1 => 2, 3 => 4].reject { false }.class.should == MyHash
    MyHash[1 => 2, 3 => 4].reject { true }.class.should == MyHash
  end
  
  it "processes entries with the same order as reject!" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}

    reject_pairs = []
    reject_bang_pairs = []
    h.dup.reject { |*pair| reject_pairs << pair }
    h.reject! { |*pair| reject_bang_pairs << pair }

    reject_pairs.should == reject_bang_pairs
  end
end

describe "Hash#reject!" do
  it "is equivalent to delete_if if changes are made" do
    {:a => 2}.reject! { |k,v| v > 1 }.should == ({:a => 2}.delete_if { |k, v| v > 1 })

    h = {1 => 2, 3 => 4}
    all_args_reject = []
    all_args_delete_if = []
    h.dup.reject! { |*args| all_args_reject << args }
    h.dup.delete_if { |*args| all_args_delete_if << args }
    all_args_reject.should == all_args_delete_if
  end
  
  it "returns nil if no changes were made" do
    { :a => 1 }.reject! { |k,v| v > 1 }.should == nil
  end
  
  it "processes entries with the same order as delete_if" do
    h = {:a => 1, :b => 2, :c => 3, :d => 4}

    reject_bang_pairs = []
    delete_if_pairs = []
    h.dup.reject! { |*pair| reject_bang_pairs << pair }
    h.dup.delete_if { |*pair| delete_if_pairs << pair }

    reject_bang_pairs.should == delete_if_pairs
  end  

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.reject! { false } }
    should_raise(TypeError) { empty.reject! { true } }
  end

  it_behaves_like(hash_iteration_method, :reject!)
end

describe "Hash#replace" do
  it_behaves_like(hash_replace, :replace)
end

describe "Hash#select" do
  it "yields two arguments: key and value" do
    all_args = []
    {1 => 2, 3 => 4}.select { |*args| all_args << args }
    all_args.should == [[1, 2], [3, 4]]
  end
  
  it "returns an array of entries for which block is true" do
    { :a => 9, :c => 4, :b => 5, :d => 2 }.select { |k,v| v % 2 == 0 }
  end

  it "processes entries with the same order as reject" do
    h = { :a => 9, :c => 4, :b => 5, :d => 2 }
    
    select_pairs = []
    reject_pairs = []
    h.dup.select { |*pair| select_pairs << pair }
    h.reject { |*pair| reject_pairs << pair }
    
    select_pairs.should == reject_pairs
  end

  it_behaves_like(hash_iteration_method, :select)
end

describe "Hash#shift" do
  it "removes a pair from hash and return it (same order as to_a)" do
    h = { :a => 1, :b => 2, "c" => 3, nil => 4, [] => 5 }
    pairs = h.to_a
    
    h.size.times do
      r = h.shift
      r.class.should == Array
      r.should == pairs.shift
      h.size.should == pairs.size
    end
    
    h.should == {}
    h.shift.should == nil
  end
  
  it "returns (computed) default for empty hashes" do
    Hash.new(5).shift.should == 5
    h = Hash.new { |*args| args }
    h.shift.should == [h, nil]
  end

  it "raises TypeError if called on a frozen instance" do
    should_raise(TypeError) { hash.shift }
    should_raise(TypeError) { empty.shift }
  end
end

describe "Hash#size" do
  it_behaves_like(hash_length, :size)
end

describe "Hash#sort" do
  it "converts self to a nested array of [key, value] arrays and sort with Array#sort" do
    { 'a' => 'b', '1' => '2', 'b' => 'a' }.sort.should == [["1", "2"], ["a", "b"], ["b", "a"]]
  end
  
  it "works when some of the keys are themselves arrays" do
    { [1,2] => 5, [1,1] => 5 }.sort.should == [[[1,1],5], [[1,2],5]]
  end
  
  it "uses block to sort array if passed a block" do
    { 1 => 2, 2 => 9, 3 => 4 }.sort { |a,b| b <=> a }.should == [[3, 4], [2, 9], [1, 2]]
  end
end

describe "Hash#store" do
  it_behaves_like(hash_store, :store)
end

describe "Hash#to_a" do
  it "returns a list of [key, value] pairs with same order as each()" do
    h = {:a => 1, 1 => :a, 3 => :b, :b => 5}
    pairs = []

    h.each do |pair|
      pairs << pair
    end
    
    h.to_a.class.should == Array
    h.to_a.should == pairs
  end
end

describe "Hash#to_hash" do
  it "returns self" do
    h = {}
    h.to_hash.equal?(h).should == true
  end
end

describe "Hash#to_s" do
  it "returns a string by calling Hash#to_a and using Array#join with default separator" do
    h = { :fun => 'x', 1 => 3, nil => "ok", [] => :y }
    h.to_a.to_s.should == h.to_s
    old, $, = $,, ':'
    h.to_a.to_s.should == h.to_s
    $, = old
  end
end

describe "Hash#update" do
  it_behaves_like(hash_update, :update)

  it_behaves_like(hash_iteration_method, :update)
end

describe "Hash#value?" do
  it_behaves_like(hash_value_p, :value?)
end

describe "Hash#values" do
  it "returns an array of values" do
    h = { 1 => :a, 'a' => :a, 'the' => 'lang'}
    h.values.class.should == Array
    h.values.sort {|a, b| a.to_s <=> b.to_s}.should == [:a, :a, 'lang']
  end
end

describe "Hash#values_at" do
  it "returns an array of values for the given keys" do
    h = {:a => 9, :b => 'a', :c => -10, :d => nil}
    h.values_at().class.should == Array
    h.values_at().should == []
    h.values_at(:a, :d, :b).class.should == Array
    h.values_at(:a, :d, :b).should == [9, nil, 'a']
  end
end
