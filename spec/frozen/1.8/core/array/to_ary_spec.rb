require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#to_ary" do
  it "returns self" do
    a = [1, 2, 3]
    a.should equal(a.to_ary)
    a = ArraySpecs::MyArray[1, 2, 3]
    a.should equal(a.to_ary)
  end
end
