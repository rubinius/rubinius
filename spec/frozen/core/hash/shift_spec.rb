require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
  end

  it "returns nil from an empty hash " do
    {}.shift.should == nil
  end

  it "returns (computed) default for empty hashes" do
    Hash.new(5).shift.should == 5
    h = Hash.new { |*args| args }
    h.shift.should == [h, nil]
  end

  ruby_version_is "" ... "1.9" do
    it "raises a TypeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.shift  }.should raise_error(TypeError)
      lambda { HashSpecs.empty_frozen_hash.shift }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.shift  }.should raise_error(RuntimeError)
      lambda { HashSpecs.empty_frozen_hash.shift }.should raise_error(RuntimeError)
    end
  end
end
