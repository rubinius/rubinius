require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#all?" do

  before :each do
    @enum = EnumerableSpecs::Numerous.new
    @empty = EnumerableSpecs::Empty.new()
    @enum1 = [0, 1, 2, -1]
    @enum2 = [nil, false, true]
  end

  it "always returns true on empty enumeration" do
    @empty.all?.should == true
    @empty.all? { nil }.should == true

    [].all?.should == true
    [].all? { false }.should == true

    {}.all?.should == true
    {}.all? { nil }.should == true
  end

  it "raises an ArgumentError when any arguments provided" do
    lambda { @enum.all?(Proc.new {}) }.should raise_error(ArgumentError)
    lambda { @enum.all?(nil) }.should raise_error(ArgumentError)
    lambda { @empty.all?(1) }.should raise_error(ArgumentError)
    lambda { @enum1.all?(1) {} }.should raise_error(ArgumentError)
    lambda { @enum2.all?(1, 2, 3) {} }.should raise_error(ArgumentError)
  end

  it "raises NoMethodError if there is no #each method defined" do
    lambda { EnumerableSpecs::NoEach.new.all? }.should raise_error(NoMethodError)
    lambda { EnumerableSpecs::NoEach.new.all? {} }.should raise_error(NoMethodError)
  end

  it "does not hide exceptions out of #each" do
    lambda {
      EnumerableSpecs::ThrowingEach.new.all?
    }.should raise_error(RuntimeError)

    lambda {
      EnumerableSpecs::ThrowingEach.new.all? { false }
    }.should raise_error(RuntimeError)
  end

  describe "with no block" do
    it "returns true if no elements are false or nil" do
      @enum.all?.should == true
      @enum1.all?.should == true
      @enum2.all?.should == false

      EnumerableSpecs::Numerous.new('a','b','c').all?.should == true
      EnumerableSpecs::Numerous.new(0, "x", true).all?.should == true
    end

    it "returns false if there are false or nil elements" do
      EnumerableSpecs::Numerous.new(false).all?.should == false
      EnumerableSpecs::Numerous.new(false, false).all?.should == false

      EnumerableSpecs::Numerous.new(nil).all?.should == false
      EnumerableSpecs::Numerous.new(nil, nil).all?.should == false

      EnumerableSpecs::Numerous.new(1, nil, 2).all?.should == false
      EnumerableSpecs::Numerous.new(0, "x", false, true).all?.should == false
      @enum2.all?.should == false
    end
  end

  describe "with block" do
    it "returns true if the block never returns false or nil" do
      @enum.all? { true }.should == true
      @enum1.all?{ |o| o < 5 }.should == true
      @enum1.all?{ |o| 5 }.should == true
    end

    it "returns false if the block ever returns false or nil" do
      @enum.all? { false }.should == false
      @enum.all? { nil }.should == false
      @enum1.all?{ |o| o > 2 }.should == false

      EnumerableSpecs::Numerous.new.all? { |i| i > 5 }.should == false
      EnumerableSpecs::Numerous.new.all? { |i| i == 3 ? nil : true }.should == false
    end

    it "stops iterating once the return value is determined" do
      yielded = []
      EnumerableSpecs::Numerous.new(:one, :two, :three).all? do |e|
        yielded << e
        false
      end.should == false
      yielded.should == [:one]

      yielded = []
      EnumerableSpecs::Numerous.new(true, true, false, true).all? do |e|
        yielded << e
        e
      end.should == false
      yielded.should == [true, true, false]

      yielded = []
      EnumerableSpecs::Numerous.new(1, 2, 3, 4, 5).all? do |e|
        yielded << e
        e
      end.should == true
      yielded.should == [1, 2, 3, 4, 5]
    end

    it "does not hide exceptions out of the block" do
      lambda {
        @enum.all? { raise "from block" }
      }.should raise_error(RuntimeError)
    end
  end
end
