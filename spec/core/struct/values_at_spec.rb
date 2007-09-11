require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#values_at" do
  it "returns an array of values" do
    clazz = Struct.new(:name, :director, :year)
    movie = clazz.new('Sympathy for Mr. Vengence', 'Chan-wook Park', 2002)
    movie.values_at(0, 1).should == ['Sympathy for Mr. Vengence', 'Chan-wook Park']
    movie.values_at(0..2).should == ['Sympathy for Mr. Vengence', 'Chan-wook Park', 2002]
  end

  it "fails when passed unsupported types" do
    car = Struct::Car.new('Ford', 'Ranger')
    should_raise(TypeError) { car.values_at('name', 'director') }
    should_raise(TypeError) { car.values_at(Class) }
    should_raise(IndexError) { car.values_at(:name, :director) }
  end
end
