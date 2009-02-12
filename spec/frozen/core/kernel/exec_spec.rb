require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#exec" do  
  it "is a private method" do
    Kernel.should have_private_instance_method(:exec)
  end
  
  it "raises a SystemCallError if cmd cannot execute" do
    lambda { exec "" }.should raise_error(SystemCallError)
  end  
end

describe "Kernel.exec" do
  it "needs to be reviewed for spec completeness"
end
