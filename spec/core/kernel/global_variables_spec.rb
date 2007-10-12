require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.global_variables" do
  it "should find subset starting with std" do
    global_variables.grep(/std/).should_include("$stderr", "$stdin", "$stdout")
    a = global_variables.size
    global_variables.include?("$foolish_global_var").should == false
    eval("$foolish_global_var = 1")
    global_variables.size.should == a+1
    global_variables.should_include("$foolish_global_var")
  end
end
