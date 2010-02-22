require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#<=>" do
  it "returns 1 if the first argument is a point in time after the second argument" do
    (Time.now <=> Time.at(0)).should == 1
    (Time.at(0, 100) <=> Time.at(0, 0)).should == 1
    (Time.at(1202778512, 100) <=> Time.at(1202778512, 99)).should == 1
  end
  
  it "returns 0 if time is the same as other" do
    (Time.at(1202778513) <=> Time.at(1202778513)).should == 0
    (Time.at(100, 100) <=> Time.at(100, 100)).should == 0
  end
  
  it "returns -1 if the first argument is a point in time before the second argument" do
    (Time.at(0) <=> Time.now).should == -1
    (Time.at(0, 0) <=> Time.at(0, 100)).should == -1
    (Time.at(100, 100) <=> Time.at(101, 100)).should == -1
  end

  # see [ruby-core:15333]
  it "returns nil when Time is compared to Numeric" do
    (Time.at(100) <=> 100).should == nil
    (Time.at(100) <=> 100.0).should == nil
  end

  it "returns nil when Time is compared to some Object" do
    (Time.at(100) <=> Object.new).should == nil
  end
end
