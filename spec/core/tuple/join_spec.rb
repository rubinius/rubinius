require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Tuple#join" do
  it "returns a string of the tuple elements separated by the separator string" do
    Tuple['a', :b, 2, '3'].join('-*-').should == "a-*-b-*-2-*-3"
  end
  
  it "defaults to calling to_s on the tuple elements" do
    t = Tuple[TupleSpecs::TupleElement.new, TupleSpecs::TupleElement.new]
    t.join(' ').should == "zonkers zonkers"
  end
  
  it "calls the specified method on the tuple elements" do
    t = Tuple[TupleSpecs::TupleElement.new, TupleSpecs::TupleElement.new]
    t.join(' ', :stringify).should == "bonkers bonkers"
  end
end
