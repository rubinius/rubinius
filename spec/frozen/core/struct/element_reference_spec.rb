require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct[]" do
  it "is a synonym for new" do
    Struct::Ruby['2.0', 'i686'].class.should == Struct::Ruby
  end
end

describe "Struct#[]" do
  it "returns the attribute referenced" do
    car = Struct::Car.new('Ford', 'Ranger', 1983)
    car['make'].should == 'Ford'
    car['model'].should == 'Ranger'
    car['year'].should == 1983
    car[:make].should == 'Ford'
    car[:model].should == 'Ranger'
    car[:year].should == 1983
    car[0].should == 'Ford'
    car[1].should == 'Ranger'
    car[2].should == 1983
    car[-3].should == 'Ford'
    car[-2].should == 'Ranger'
    car[-1].should == 1983
  end

  it "fails when it does not know about the requested attribute" do
    car = Struct::Car.new('Ford', 'Ranger')
    lambda { car[3]        }.should raise_error(IndexError)
    lambda { car[-4]       }.should raise_error(IndexError)
    lambda { car[:body]    }.should raise_error(NameError)
    lambda { car['wheels'] }.should raise_error(NameError)
  end

  it "fails if passed too many arguments" do
    car = Struct::Car.new('Ford', 'Ranger')
    lambda { car[:make, :model] }.should raise_error(ArgumentError)
  end

  it "fails if not passed a string, symbol, or integer" do
    car = Struct::Car.new('Ford', 'Ranger')
    lambda { car[Object.new] }.should raise_error(TypeError)
  end
end
