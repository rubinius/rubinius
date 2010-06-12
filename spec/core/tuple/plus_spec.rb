require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#+" do
  it "returns the concatenation of two tuples" do
    t1 = Rubinius::Tuple[:a, 'three', 4]
    t2 = Rubinius::Tuple[:b, 'two', 5, 6]
    t = t1 + t2
    t[0].should == :a
    t[1].should == 'three'
    t[2].should == 4
    t[3].should == :b
    t[4].should == 'two'
    t[5].should == 5
    t[6].should == 6
    t.size.should == 7
  end
end
