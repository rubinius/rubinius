require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#each_pair" do
  it "passes each key value pair to the given block" do
    car = Struct::Car.new('Ford', 'Ranger', 2001)
    car.each_pair do |key, value|
      value.should == car[key]
    end
  end

  it "fails if not passed a block" do
    car = Struct::Car.new('Ford', 'Ranger')
    lambda { car.each_pair }.should raise_error(LocalJumpError)
  end
end
