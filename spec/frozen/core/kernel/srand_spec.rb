require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.srand" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:srand)
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

  it "defaults number to a random value" do
    lambda { srand }.should_not raise_error
    srand.should_not == 0
  end

  it "calls #to_i on number" do
    srand(3.8)
    srand.should == 3
  end
end

describe "Kernel#srand" do
  it "needs to be reviewed for spec completeness"
end
