require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#delete" do
  it "removes the entry and returns the deleted value" do
    h = {:a => 5, :b => 2}
    h.delete(:b).should == 2
    h.should == {:a => 5}
  end

  it "calls eql? on colliding keys it finds" do
    h = Hash.new

    # a class that tracks how many times eql? is called
    eql_counter = Class.new do
      def eql_count; @eql_count ||= 0; end
      def eql_count=(count); @eql_count = count; end
      # defer to super, so both get inserted
      def eql?(obj); self.eql_count += 1; super; end
    end

    # two equals_counter objects with the same hash
    obj1 = eql_counter.new
    obj2 = eql_counter.new
    def obj1.hash; 0; end
    def obj2.hash; 0; end

    h[obj1] = :a
    h[obj2] = :b

    # obj2 has the same hash, so eql? should have been called once
    obj2.eql_count.should == 1

    # now always return true, so first key encountered is deleted
    def obj1.eql?(obj); self.eql_count += 1; true; end
    def obj2.eql?(obj); self.eql_count += 1; true; end

    # delete one of them
    h.delete(obj1)

    # assert that between the two objects, eql? was called twice total
    # we can't assert specific counts since we shouldn't specify bucket ordering
    (obj1.eql_count + obj2.eql_count).should == 2
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
