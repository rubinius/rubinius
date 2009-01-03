require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#+@" do    
  it "returns self" do
    obj = NumericSub.new
    obj.send(:+@).should == obj
  end
end
