require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#abs" do
  before(:each) do
    @obj = NumericSub.new
  end
  
  it "returns self when self is greater than 0" do
    @obj.should_receive(:<).with(0).and_return(false)
    @obj.abs.should == @obj
  end
  
  it "returns self\#@- when self is less than 0" do
    @obj.should_receive(:<).with(0).and_return(true)
    @obj.should_receive(:-@).and_return(:absolute_value)
    @obj.abs.should == :absolute_value
  end
end