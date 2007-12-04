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
    lambda { car[:something] = true }.should raise_error(NameError)
    lambda { car[:dogtown] = true   }.should raise_error(NameError)
    lambda { car[100] = true        }.should raise_error(IndexError)
    lambda { car[Time.now] = true   }.should raise_error(TypeError)
    lambda { car[Class] = true      }.should raise_error(TypeError)
  end
end
