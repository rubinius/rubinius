require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Tuple#join" do
  it "returns a string of the tuple elements separated by the separator string" do
    Rubinius::Tuple['a', :b, 2, '3'].join('-*-').should == "a-*-b-*-2-*-3"
  end
  
  it "defaults to calling to_s on the tuple elements" do
    t = Rubinius::Tuple[TupleSpecs::TupleElement.new, TupleSpecs::TupleElement.new]
    t.join(' ').should == "zonkers zonkers"
  end
  
  it "calls the specified method on the tuple elements" do
    t = Rubinius::Tuple[TupleSpecs::TupleElement.new, TupleSpecs::TupleElement.new]
    t.join(' ', :stringify).should == "bonkers bonkers"
  end
end
