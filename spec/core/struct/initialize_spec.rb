require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#initialize" do
  
  it "is private" do
    Struct::Car.private_instance_methods.map { |m| m.to_s }.include?("initialize").should == true
  end
  
  it "does nothing when passed a set of fields equal to self" do
    car = same_car = Struct::Car.new("Honda", "Accord", "1998")
    car.instance_eval { initialize("Honda", "Accord", "1998") }
    car.should == same_car
  end
  
end