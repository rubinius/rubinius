require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#push" do
  it "appends the arguments to the array" do
    a = [ "a", "b", "c" ]
    a.push("d", "e", "f").equal?(a).should == true
    a.push().should == ["a", "b", "c", "d", "e", "f"]
    a.push(5)
    a.should == ["a", "b", "c", "d", "e", "f", 5]
  end
  
compliant :r18 do
  it "raises TypeError on a frozen array if modification takes place" do
    should_raise(TypeError) { frozen_array.push(1) }
  end

  it "does not raise on a frozen array if no modification is made" do
    frozen_array.push() # ok
  end
end

noncompliant :rubinius do
  it "always raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.push() }
    should_raise(TypeError) { frozen_array.push(1) }
  end
end
end
