require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../mspec'

describe MSpec, ".protect" do
  it "rescues any exceptions raised when executing the block argument" do
    lambda {
      MSpec.protect("") { raise Exception, "Now you see me..." }
    }.should_not raise_error
  end
end

describe MSpec, ".stack" do
  it "returns an array" do
    MSpec.stack.should be_kind_of(Array)
  end
end

describe MSpec, ".current" do
  it "returns the top of the execution stack" do
    MSpec.stack.clear
    MSpec.stack.push :a
    MSpec.stack.push :b
    MSpec.current.should == :b
  end
end
