require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#dup" do
  it "returns an Array or a subclass instance" do
    [].dup.class.should == Array
    MyArray[1, 2].dup.class.should == MyArray
  end

  it "produces a shallow copy where the references are directly copied" do
    a = [mock('1'), mock('2')]
    b = a.dup
    b.first.object_id.should == a.first.object_id
    b.last.object_id.should == a.last.object_id
  end
end
