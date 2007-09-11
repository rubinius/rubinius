require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#values" do
  it "is a synonym for to_a" do
    car = Struct::Car.new('Nissan', 'Maxima')
    car.values.should == car.to_a

    Struct::Car.new.values.should == Struct::Car.new.to_a
  end
end
