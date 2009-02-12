require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#test" do
  before :all do
    @file = File.dirname(__FILE__) + '/fixtures/classes.rb'
    @dir = File.dirname(__FILE__) + '/fixtures'
  end
  
  it "is a private method" do
    Kernel.should have_private_instance_method(:test)
  end
  
  it "returns true when passed ?f if the argument is a regular file" do
    Kernel.test(?f, @file).should == true
  end
  
  it "returns true when passed ?e if the argument is a file" do
    Kernel.test(?e, @file).should == true
  end
  
  it "returns true when passed ?d if the argument is a directory" do
    Kernel.test(?d, @dir).should == true
  end
end

describe "Kernel.test" do
  it "needs to be reviewed for spec completeness"
end
