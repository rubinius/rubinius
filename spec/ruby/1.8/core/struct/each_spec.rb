require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#each" do
  it "passes each value to the given block" do
    car = Struct::Car.new('Ford', 'Ranger')
    i = -1
    car.each do |value|
      value.should == car[i += 1]
    end
  end

  it "fails if not passed a block" do
    car = Struct::Car.new('Ford', 'Ranger')
    lambda { car.each }.should raise_error(LocalJumpError)
  end
end
