require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#dup" do
  it "returns a copy of tuple" do
    t = Rubinius::Tuple[:a, 'three', 4].dup
    t[0].should == :a
    t[1].should == 'three'
    t[2].should == 4
    t.size.should == 3
  end
end
