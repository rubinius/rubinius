require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#size" do
  it "is a synonym for length" do
    Struct::Car.new.size.should == Struct::Car.new.length
  end
end
