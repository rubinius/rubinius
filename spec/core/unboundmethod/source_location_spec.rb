require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "UnboundMethod#source_location" do
  before(:each) do
    @method = MethodSpecs::SourceLocation.instance_method(:location)
  end

  it "returns an Array" do
    @method.source_location.should be_an_instance_of(Array)
  end

  it "sets the first value to the path of the file in which the method was defined" do
    file = @method.source_location.first
    file.should be_an_instance_of(String)
    file.should == File.dirname(__FILE__) + '/fixtures/classes.rb'
  end

  it "sets the last value to a Fixnum representing the line on which the method was defined" do
    line = @method.source_location.last 
    line.should be_an_instance_of(Fixnum)
    line.should == 5 
  end

  it "returns the last place the method was defined" do
    MethodSpecs::SourceLocation.instance_method(:redefined).source_location.last.should == 13
  end

  it "returns the location of the original method even if it was aliased" do
    MethodSpecs::SourceLocation.instance_method(:aka).source_location.last.should == 17
  end
end
