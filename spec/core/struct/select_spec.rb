require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#select" do

  it "raises ArgumentError if given any non-block arguments" do
    should_raise(ArgumentError) do
      Struct::Car.new.select(1) { }
    end
  end
  
  it "returns a new array of elements for which block is true" do
    struct = Struct::Car.new("Toyota", "Tercel", "2000")
    struct.select { |i| i == "2000" }.should == [ "2000" ]
  end
  
  it "returns an instance of Array" do
    struct = Struct::Car.new("Ford", "Escort", "1995")
    struct.select { true }.class.should == Array
  end

end