require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Struct#size" do
  it "is a synonym for length" do
    Struct::Car.new.size.should == Struct::Car.new.length
  end
end
