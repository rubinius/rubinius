require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Numeric#negative?" do
  before(:each) do
    @obj = NumericSpecs::Subclass.new
  end

  it "returns true if self is less than 0" do
    @obj.should_receive(:<).with(0).and_return(true)
    @obj.negative?.should == true
  end

  it "returns false if self is greater than 0" do
    @obj.should_receive(:<).with(0).and_return(false)
    @obj.negative?.should == false
  end
end
