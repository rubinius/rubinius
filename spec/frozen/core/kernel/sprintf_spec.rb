require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#sprintf" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:sprintf)
  end
  
  it "treats nil arguments as zero-width strings in %s slots" do
    sprintf("%s%d%s%s", nil, 4, 'a', 'b').should == '4ab'
  end

  it "treats nil arguments as zeroes in %d slots" do
    sprintf("%d%d%s%s", nil, 4, 'a', 'b').should == '04ab'
  end
end

describe "Kernel.sprintf" do
  it "needs to be reviewed for spec completeness"
end
