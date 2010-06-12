require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#to_tuple" do
  it "returns an instance of Rubinius::Tuple" do
    [1, 2].to_tuple.should be_an_instance_of(Rubinius::Tuple)
  end

  it "returns a Rubinius::Tuple containing only the elements in the array" do
    array = [1, 2, 3, 4]
    array.pop
    array.shift
    tuple = array.to_tuple
    tuple.size.should == 2
    tuple[0].should == 2
    tuple[1].should == 3
  end
end
