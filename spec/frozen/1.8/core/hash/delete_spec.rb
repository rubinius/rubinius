require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

def make_hash_and_keys
  h = Hash.new
  key1 = Object.new
  key2 = Object.new
  def key1.hash; 0; end
  def key2.hash; 0; end

  h[key1] = :val1
  h[key2] = :val2

  return h, key1, key2
end

describe "Hash#delete" do
  it "removes the entry and returns the deleted value" do
    h = {:a => 5, :b => 2}
    h.delete(:b).should == 2
    h.should == {:a => 5}
  end

  # TODO: these 2 specs use Object, we prolly need String as well, maybe
  # Symbol and Fixnum. Those are pending a talk between Evan and Matz.

  it "uses eql? on colliding keys to determine deletion" do
    h, key1, key2 = make_hash_and_keys
    h.size.should == 2

    def key1.eql?(obj); true; end
    def key2.eql?(obj); true; end

    h.delete(key1).should_not be_nil

    h.size.should == 1
  end

  it "uses eql? on non-colliding keys to determine deletion" do
    h, key1, key2 = make_hash_and_keys
    h.size.should == 2

    def key1.eql?(obj); false; end
    def key2.eql?(obj); false; end

    h.delete(key1).should_not be_nil

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

  compliant_on :ruby, :jruby do
    it "raises a TypeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.delete("foo")  }.should raise_error(TypeError)
      lambda { HashSpecs.empty_frozen_hash.delete("foo") }.should raise_error(TypeError)
    end
  end
end
