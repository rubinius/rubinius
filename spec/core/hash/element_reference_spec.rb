require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
    lambda { Hash[1, 2, 3] }.should raise_error(ArgumentError)
  end
 
  it "raises when mixing argument styles" do
    lambda { Hash[1, 2, {3 => 4}] }.should raise_error(ArgumentError)
    Hash[1, 2, 3, {3 => 4}].should == {1 => 2, 3 => {3 => 4}}
  end
  
  it "does not call to_hash" do
    obj = Object.new
    def obj.to_hash() { 1 => 2, 3 => 4 } end
    lambda { Hash[obj] }.should raise_error(ArgumentError)
  end

  it "returns an instance of the class it's called on" do
    Hash[MyHash[1, 2]].class.should == Hash
    MyHash[Hash[1, 2]].class.should == MyHash
  end
end

describe "Hash#[]" do
  it "returns the value for key" do
    obj = Object.new
    h = { 1 => 2, 3 => 4, "foo" => "bar", obj => obj, [] => "baz" }
    h[1].should == 2
    h[3].should == 4
    h["foo"].should == "bar"
    h[obj].should == obj
    h[[]].should == "baz"
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
    def x.hash() 0 end

    { }[x].should == nil
  end

  # True for all other methods using hash codes as well...
  it "calls % on hash code from hash()" do
    hash_code = mock('hash code')
    hash_code.should_receive(:%).and_return(2)
    key = mock('key')
    key.should_receive(:hash).and_return(hash_code)

    h = {1 => 2}
    h[key].should == nil
  end

  it "does not compare key with unknown hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = Object.new
    y = Object.new
    def x.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() 0 end
    def y.hash() 1 end

    { y => 1 }[x].should == nil
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
