require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Tuple#shift" do
  it "returns the tuple after removing the first value" do
    t = Rubinius::Tuple[99, 98, 97].shift
    t[0].should == 98
    t[1].should == 97
    t.size.should == 2
  end

  it "does not modify tuple if it has zero size" do
    t = Rubinius::Tuple.new(0)
    t.shift.should == t
  end
end
