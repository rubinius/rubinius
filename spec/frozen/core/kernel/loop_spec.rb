require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.loop" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:loop)
  end
  
  it "calls block until it is terminated by a break" do
    i = 0
    loop do
      i += 1
      break if i == 10
    end

    i.should == 10
  end

  it "returns value passed to break" do
    loop do
      break 123
    end.should == 123
  end

  it "returns nil if no value passed to break" do
    loop do
      break
    end.should == nil
  end

  it "raises a LocalJumpError if no block given" do
    lambda { loop }.should raise_error(LocalJumpError)
  end
end

describe "Kernel#loop" do
  it "needs to be reviewed for spec completeness"
end
