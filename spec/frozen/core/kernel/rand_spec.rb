require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.rand" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:rand)
  end
  
  it "returns a random float less than 1 if no max argument is passed" do
    rand.kind_of?(Float).should == true
  end

  it "returns a random int or bigint less than the argument for an integer argument" do
    rand(77).kind_of?(Integer).should == true
  end

  it "returns a random integer less than the argument casted to an int for a float argument greater than 1" do
    rand(1.3).kind_of?(Integer).should == true
  end

  it "returns a random float less than 1 for float arguments less than 1" do
    rand(0.01).kind_of?(Float).should == true
  end

  it "never returns a value greater or equal to 1.0 with no arguments" do
    1000.times do
      (rand < 1.0).should == true
    end
  end

  it "never returns a value greater or equal to any passed in max argument" do
    1000.times do
      (rand(100) < 100).should == true
    end
  end
end

describe "Kernel#rand" do
  it "needs to be reviewed for spec completeness"
end
