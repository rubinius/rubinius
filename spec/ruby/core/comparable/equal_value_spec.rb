require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Comparable#==" do
  before(:each) do
    @a = ComparableSpecs::Weird.new(0)
    @b = ComparableSpecs::Weird.new(10)
  end

  it "returns true if other is the same as self" do
    (@a == @a).should be_true
    (@b == @b).should be_true
  end

  it "calls #<=> on self with other and returns true if #<=> returns 0" do
    @a.should_receive(:<=>).any_number_of_times.and_return(0)
    (@a == @b).should be_true
  end

  it "calls #<=> on self with other and returns true if #<=> returns 0.0" do
    @a.should_receive(:<=>).any_number_of_times.and_return(0.0)
    (@a == @b).should be_true
  end

  it "returns false if calling #<=> on self returns a positive Integer" do
    @a.should_receive(:<=>).any_number_of_times.and_return(1)
    (@a == @b).should be_false
  end

  it "returns false if calling #<=> on self returns a negative Integer" do
    @a.should_receive(:<=>).any_number_of_times.and_return(-1)
    (@a == @b).should be_false
  end

  it "returns false if calling #<=> on self returns nil" do
    @a.should_receive(:<=>).any_number_of_times.and_return(nil)
    (@a == @b).should be_false
  end

  it "raises an ArgumentError if calling #<=> on self returns a non-Integer" do
    @a.should_receive(:<=>).any_number_of_times.and_return("abc")
    lambda { @a == @b }.should raise_error(ArgumentError)
  end

  context "when #<=> is not defined" do
    before :each do
      @a = ComparableSpecs::WithoutCompareDefined.new
      @b = ComparableSpecs::WithoutCompareDefined.new
    end

    it "returns true for identical objects" do
      @a.should == @a
    end

    it "returns false and does not recurse infinitely" do
      @a.should_not == @b
    end
  end

  context "when #<=> calls super" do
    before :each do
      @a = ComparableSpecs::CompareCallingSuper.new
      @b = ComparableSpecs::CompareCallingSuper.new
    end

    it "returns true for identical objects" do
      @a.should == @a
    end

    it "calls the defined #<=> only once for different objects" do
      @a.should_not == @b
      @a.calls.should == 1
    end
  end
end
