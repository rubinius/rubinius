require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
