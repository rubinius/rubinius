require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#modulo" do 
  it "returns the result of calling self#% with other" do
    obj = NumericSub.new
    obj.should_receive(:%).with(20).and_return(:result)
    
    obj.modulo(20).should == :result
  end
end
