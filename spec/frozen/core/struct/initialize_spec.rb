require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#initialize" do

  it "is private" do
    Struct::Car.should have_private_instance_method(:initialize)
  end

  it "does nothing when passed a set of fields equal to self" do
    car = same_car = Struct::Car.new("Honda", "Accord", "1998")
    car.instance_eval { initialize("Honda", "Accord", "1998") }
    car.should == same_car
  end

  it "explicitly sets instance variables to nil when args not provided to initialize" do
    car = Honda.new
    car.make.should == nil # still nil despite override in Honda#initialize b/c of super order
  end
end
