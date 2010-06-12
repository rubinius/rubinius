require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Tuple#join_upto" do
  it "returns a string other specified number of tuple elements separated by the separator string" do
    Rubinius::Tuple[1, 2, 3, 4].join_upto(',', 2).should == "1,2"
  end
  
  it "returns an empty string if called on an empty tuple" do
    Rubinius::Tuple.new(0).join_upto(' ', 2).should == ""
  end
  
  it "joins all the elements if the upto index is greater than the number of elements" do
    Rubinius::Tuple[1, 2, 3].join_upto('', 6).should == "123"
  end
  
  it "defaults to calling to_s on the tuple elements" do
    t = Rubinius::Tuple[TupleSpecs::TupleElement.new, TupleSpecs::TupleElement.new]
    t.join_upto(' ', 1).should == "zonkers"
    t.join_upto('.', 0).should == ""
  end
  
  it "calls the specified method on the tuple elements" do
    t = Rubinius::Tuple[TupleSpecs::TupleElement.new, TupleSpecs::TupleElement.new]
    t.join_upto(' ', 1, :stringify).should == "bonkers"
    t.join_upto('.', 0, :stringify).should == ""
  end
end
