require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/equal_value'

describe "Struct#eql?" do
  it_behaves_like(:struct_equal_value, :eql?)

  it "returns false if any corresponding elements are not #eql?" do
    car = Struct::Car.new("Honda", "Accord", 1998)
    similar_car = Struct::Car.new("Honda", "Accord", 1998.0)
    car.send(@method, similar_car).should be_false
  end
end
