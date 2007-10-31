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

  it "isn't confused by previous shift" do
    a = [ "a", "b", "c" ]
    a.shift
    a.push("foo")
    a.should == ["b", "c", "foo"]
  end
  
  compliant :mri do
    it "raises TypeError on a frozen array if modification takes place" do
      should_raise(TypeError) { @frozen_array.push(1) }
    end

    it "does not raise on a frozen array if no modification is made" do
      @frozen_array.push() # ok
    end
  end
end
