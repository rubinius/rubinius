require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct[]" do
  it "is a synonym for new" do
    Struct::Ruby['2.0', 'i686'].class.should == Struct::Ruby
  end
end

describe "Struct#[]" do
  it "returns the attribute referenced" do
    car = Struct::Car.new('Ford', 'Ranger')
    car['make'].should == 'Ford'
    car['model'].should == 'Ranger'
    car[:make].should == 'Ford'
    car[:model].should == 'Ranger'
    car[0].should == 'Ford'
    car[1].should == 'Ranger'
  end

  it "fails when it does not know about the requested attribute" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(IndexError) { car[5] }
    should_raise(NameError) { car[:body] }
    should_raise(NameError) { car['wheels'] }
  end

  it "fails if passed too many arguments" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(ArgumentError) { car[:make, :model] }
  end

  it "fails if not passed a string, symbol, or integer" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(TypeError) { car[Time.now] }
    should_raise(TypeError) { car[ { :name => 'chris' } ] }
    should_raise(TypeError) { car[ ['chris', 'evan'] ] }
    should_raise(TypeError) { car[ Class ] }
  end
end
