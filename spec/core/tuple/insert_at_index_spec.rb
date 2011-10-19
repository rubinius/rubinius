require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#insert_at_index" do
  before :each do
    @tuple = Rubinius::Tuple[1, 2]
  end

  it "returns a new Tuple inserting an item into a zero-element Tuple" do
    Rubinius::Tuple.new(0).insert_at_index(0, 1).should == Rubinius::Tuple[1]
  end

  it "returns a new Tuple inserting an item at the front" do
    @tuple.insert_at_index(0, 0).should == Rubinius::Tuple[0, 1, 2]
  end

  it "returns a new Tuple inserting an item in the middle" do
    @tuple.insert_at_index(1, 0).should == Rubinius::Tuple[1, 0, 2]
  end

  it "returns a new Tuple inserting an item at the end" do
    @tuple.insert_at_index(2, 0).should == Rubinius::Tuple[1, 2, 0]
  end

  it "raises a ObjectBoundsExceeded exception if passed a negative index" do
    lambda { @tuple.insert_at_index -1, 0 }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises a ObjectBoundsExceeded exception if passed an index larger than size" do
    lambda { @tuple.insert_at_index -1, 3 }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
