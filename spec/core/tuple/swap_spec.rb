require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#swap" do
  it "swaps the contents of two indices" do
    t = Rubinius::Tuple.new(5)
    t.put(0, 1)
    t.put(1, 2)
    t.put(2, 3)
    t.put(3, 4)
    t.put(4, 5)

    t.swap(2, 4)

    t.at(2).should == 5
    t.at(4).should == 3
  end
end
