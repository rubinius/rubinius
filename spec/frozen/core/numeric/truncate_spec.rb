require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#truncate" do
  before(:each) do
    @obj = NumericSub.new
  end
  
  it "converts self to a Float (using #to_f) and returns the #truncate'd result" do
    @obj.should_receive(:to_f).and_return(2.5555, -2.3333)
    @obj.truncate.should == 2
    @obj.truncate.should == -2
  end
end
