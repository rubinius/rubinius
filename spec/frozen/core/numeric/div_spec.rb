require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#div" do
  before(:each) do
    @obj = NumericSub.new
  end
  
  it "calls self#/ with other, converts the result to a Float (using #to_f) and returns the #floor'ed result" do
    result = mock("Numeric#div result")
    result.should_receive(:to_f).and_return(13 - TOLERANCE)
    @obj.should_receive(:/).with(10).and_return(result)
    
    @obj.div(10).should == 12
  end
end
