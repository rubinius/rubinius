require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.8.7" do
  describe "Enumerable#none?" do
    it "returns true if none of the elements in self are true" do
      e = EnumerableSpecs::Numerous.new(false, nil, false)
      e.none?.should be_true
    end

    it "returns false if at least one of the elements in self are true" do
      e = EnumerableSpecs::Numerous.new(false, nil, true, false)
      e.none?.should be_false
    end
  end

  describe "Enumerable#none? with a block" do
    before(:each) do
      @e = EnumerableSpecs::Numerous.new(1,1,2,3,4)
    end

    it "passes each element to the block in turn until it returns true" do
      acc = []
      @e.none? {|e| acc << e; false }
      acc.should == [1,1,2,3,4]
    end

    it "stops passing elements to the block when it returns true" do
      acc = []
      @e.none? {|e| acc << e; e == 3 ? true : false }
      acc.should == [1,1,2,3]
    end

    it "returns true if the block never returns true" do
      @e.none? {|e| false }.should be_true
    end

    it "returns false if the block ever returns true" do
      @e.none? {|e| e == 3 ? true : false }.should be_false
    end
  end
end
