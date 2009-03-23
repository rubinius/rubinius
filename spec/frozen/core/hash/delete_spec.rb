require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#delete" do
  it "removes the entry and returns the deleted value" do
    h = {:a => 5, :b => 2}
    h.delete(:b).should == 2
    h.should == {:a => 5}
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

  ruby_version_is "" ... "1.9" do
    it "raises a TypeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.delete("foo")  }.should raise_error(TypeError)
      lambda { HashSpecs.empty_frozen_hash.delete("foo") }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.delete("foo")  }.should raise_error(RuntimeError)
      lambda { HashSpecs.empty_frozen_hash.delete("foo") }.should raise_error(RuntimeError)
    end
  end
end
