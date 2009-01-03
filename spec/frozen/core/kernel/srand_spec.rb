require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.srand" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("srand")
  end
  
  it "srand should return the previous seed value" do
    srand(10)
    srand(20).should == 10
  end

  it "srand should seed the RNG correctly and repeatably" do
    srand(10)
    x = rand
    srand(10)
    rand.should == x
  end
end

describe "Kernel#srand" do
  it "needs to be reviewed for spec completeness"
end
