require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#binding" do
  it "returns the binding associated wiht self" do
    obj = mock('binding')
    def obj.test_binding(some, params)
      lambda {}
    end
    
    binding = obj.test_binding(1, 2)
    
    eval("some", binding).should == 1
    eval("params", binding).should == 2
  end
end