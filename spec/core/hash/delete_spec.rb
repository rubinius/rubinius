require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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

  compliant :mri do
    it "raises TypeError if called on a frozen instance" do
      should_raise(TypeError) { @hash.delete("foo") }
      should_raise(TypeError) { @empty.delete("foo") }
    end
  end
end
