require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#to_s" do
  it "is a synonym for inspect" do
    car = Struct::Car.new('Ford', 'Ranger')
    car.inspect.should == car.to_s
  end
end
