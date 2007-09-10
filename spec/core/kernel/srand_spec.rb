require File.dirname(__FILE__) + '/../../spec_helper'

context "Kernel.srand()" do
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
