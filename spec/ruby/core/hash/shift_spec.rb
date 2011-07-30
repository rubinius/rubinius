require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Hash#shift" do
  before :each do
    ScratchPad.record []
    @h = new_hash("a", 1, "b", 2, "c", 3)
  end

  it "returns nil from an empty hash " do
    new_hash.shift.should == nil
  end

  it "returns (computed) default for empty hashes" do
    new_hash(5).shift.should == 5
    h = new_hash { |*args| args }
    h.shift.should == [h, nil]
  end

  ruby_version_is "" ... "1.9" do
    it "removes an arbitrary pair from hash and return it" do
      ScratchPad << @h.shift until @h.empty?

      ScratchPad.recorded.sort.should == [["a", 1], ["b", 2], ["c", 3]]
      @h.should be_empty
    end

    it "raises a TypeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.shift  }.should raise_error(TypeError)
      lambda { HashSpecs.empty_frozen_hash.shift }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "returns the elements in the insertion order" do
      ScratchPad << @h.shift until @h.empty?
      ScratchPad.recorded.should == [["a", 1], ["b", 2], ["c", 3]]
    end

    it "raises a RuntimeError if called on a frozen instance" do
      lambda { HashSpecs.frozen_hash.shift  }.should raise_error(RuntimeError)
      lambda { HashSpecs.empty_frozen_hash.shift }.should raise_error(RuntimeError)
    end
  end
end
