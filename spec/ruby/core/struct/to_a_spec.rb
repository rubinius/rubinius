require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#to_a" do
  it "returns the values for this instance as an array" do
    Struct::Car.new('Geo', 'Metro', 1995).to_a.should == ['Geo', 'Metro', 1995]
    Struct::Car.new('Ford').to_a.should == ['Ford', nil, nil]
  end
end
