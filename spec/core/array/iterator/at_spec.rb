require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator#at for a forward iterator" do
  before :each do
    @iter = [1, 2, 3, 4].to_iter
  end

  it "raises on ObjectBoundsExceededError before #next has been called" do
    lambda { @iter.at(0) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "returns the same element as #item" do
    while @iter.next
      @iter.at(0).should == @iter.item
    end
  end

  it "returns an element at a positive offset to the current index" do
    @iter.at(1).should == 1
  end

  it "returns an element at a negative offset to the current index" do
    @iter.next
    @iter.next
    @iter.at(-1).should == 1
  end

  it "raises on ObjectBoundsExceededError when the iterator reaches the end" do
    while @iter.next
    end
    lambda { @iter.at(0) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises an ObjectBoundsExceededError if relative index is greater than the array" do
    lambda { @iter.at(5) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises an ObjectBoundsExceededError if relative index is less than 0" do
    lambda { @iter.at(-1) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end

describe "Array::Iterator#at for a reverse iterator" do
  before :each do
    @iter = [1, 2, 3, 4].to_reverse_iter
  end

  it "raises on ObjectBoundsExceededError before #rnext has been called" do
    lambda { @iter.at(0) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "returns the same element as #item at offset 0 from the current index" do
    while @iter.rnext
      @iter.at(0).should == @iter.item
    end
  end

  it "returns an element at a positive offset to the current index" do
    @iter.rnext
    @iter.rnext
    @iter.at(1).should == 4
  end

  it "returns an element at a negative offset to the current index" do
    @iter.at(-1).should == 4
  end

  it "raises on ObjectBoundsExceededError when the iterator reaches the end" do
    while @iter.next
    end
    lambda { @iter.at(0) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises an ObjectBoundsExceededError if relative index is greater than the array" do
    lambda { @iter.at(1) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises an ObjectBoundsExceededError if relative index is less than 0" do
    lambda { @iter.at(-5) }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
