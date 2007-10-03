require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.local_variables" do
  it "should contain locals as they are added" do
    a = 1
    local_variables.should == ["a"]
    b = 2
    local_variables.include?("a").should == true
    local_variables.include?("b").should == true
    # I'm not sure that the order of local variables should be in the spec for the language
    # local_variables.should == ["b","a"]
  end

  it "should be accessable from bindings" do
    def local_var_foo
      a = 1
      b = 2
      binding      
    end
    foo_binding = local_var_foo()
    res = eval("local_variables",foo_binding)
    res.include?("a").should == true
    res.include?("b").should == true
  end
end
