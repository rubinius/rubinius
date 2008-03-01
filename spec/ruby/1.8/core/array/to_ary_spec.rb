require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#to_ary" do
  it "returns self" do
    a = [1, 2, 3]
    a.equal?(a.to_ary).should == true
    a = ArraySpecs::MyArray[1, 2, 3]
    a.equal?(a.to_ary).should == true
  end
end
