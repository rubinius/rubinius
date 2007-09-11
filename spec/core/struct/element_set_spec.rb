require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#[]=" do
  it "assigns the passed value" do
    car = Struct::Car.new('Ford', 'Ranger')
    car[:model] = 'Escape'
    car[:model].should == 'Escape'
    car[1] = 'Excursion'
    car[:model].should == 'Excursion'
  end

  it "fails when trying to assign attributes which don't exist" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(NameError) { car[:something] = true }
    should_raise(NameError) { car[:dogtown] = true }
    should_raise(IndexError) { car[100] = true }
    should_raise(TypeError) { car[Time.now] = true }
    should_raise(TypeError) { car[Class] = true }
  end
end
