require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#to_a" do
  it "returns an array containing the elements of the tuple" do
    Rubinius::Tuple[:a, 1, :b, 2].to_a.should == [:a, 1, :b, 2]
    Rubinius::Tuple.new(0).to_a.should == []
  end

  it "does not skip nil values" do
    Rubinius::Tuple[:a, nil, :b, 2, nil].to_a.should == [:a, nil, :b, 2, nil]
    Rubinius::Tuple.new(2).to_a.should == [nil, nil]
  end
end
