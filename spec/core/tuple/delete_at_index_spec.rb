require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#delete_at_index" do
  before :each do
    @tuple = Rubinius::Tuple[1, 2, 3]
  end

  it "returns nil if deleting the last element" do
    Rubinius::Tuple[1].delete_at_index(0).should be_nil
  end

  it "returns a new Tuple removing an item at the front" do
    @tuple.delete_at_index(0).should == Rubinius::Tuple[2, 3]
  end

  it "returns a new Tuple removing an item in the middle" do
    @tuple.delete_at_index(1).should == Rubinius::Tuple[1, 3]
  end

  it "returns a new Tuple removing an item at the end" do
    @tuple.delete_at_index(2).should == Rubinius::Tuple[1, 2]
  end

  it "raises a ObjectBoundsExceeded exception if passed a negative index" do
    lambda { @tuple.delete_at_index -1 }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises a ObjectBoundsExceeded exception if passed an index larger than size" do
    lambda { @tuple.delete_at_index 3 }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
