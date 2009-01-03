require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#inspect" do
  it "returns a string representation of some kind" do
    car = Struct::Car.new('Ford', 'Ranger')
    car.inspect.should == '#<struct Struct::Car make="Ford", model="Ranger", year=nil>'
    Whiskey = Struct.new(:name, :ounces)
    Whiskey.new('Jack', 100).inspect.should == '#<struct Whiskey name="Jack", ounces=100>'
  end
end
