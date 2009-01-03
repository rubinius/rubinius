require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#quo" do
  it "returns the result of calling self#/ with other" do
    obj = NumericSub.new
    obj.should_receive(:/).with(19).and_return(:result)
    
    obj.quo(19).should == :result
  end
end
