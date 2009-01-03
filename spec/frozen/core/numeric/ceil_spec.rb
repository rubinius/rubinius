require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#ceil" do
  before(:each) do
    @obj = NumericSub.new
  end
  
  it "converts self to a Float (using #to_f) and returns the #ceil'ed result" do
    @obj.should_receive(:to_f).and_return(1 + TOLERANCE, - 1 - TOLERANCE)
    @obj.ceil.should == 2
    @obj.ceil.should == -1
  end
end
