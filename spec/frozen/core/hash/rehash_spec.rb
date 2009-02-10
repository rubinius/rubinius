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

    h.rehash.should equal(h)
    h.key?(k1).should == true
    h[k1].should == 0

    k1 = mock('k1')
    k2 = mock('k2')
    v1 = mock('v1')
    v2 = mock('v2')

    # Can't use should_receive here because it uses hash() internally
    def v1.hash() raise("values shouldn't be rehashed"); end
    def v2.hash() raise("values shouldn't be rehashed"); end

    h = { k1 => v1, k2 => v2 }

    def k1.hash() 0 end
    def k2.hash() 0 end

    h.rehash
    h[k1].should == v1
    h[k2].should == v2
  end

  compliant_on :rubinius do
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
  end

  it "raises a TypeError if called on a frozen instance" do
    lambda { HashSpecs.frozen_hash.rehash  }.should raise_error(TypeError)
    lambda { HashSpecs.empty_frozen_hash.rehash }.should raise_error(TypeError)
  end
end
