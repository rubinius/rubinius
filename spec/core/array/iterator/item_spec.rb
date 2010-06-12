require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator#item for a forward iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_iter
  end

  it "raises ObjectBoundsExceededError if the array is empty" do
    lambda { [].to_iter.item }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises ObjectBoundsExceededError before #next is called" do
    lambda { @iter.item }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "returns the item at the current index" do
    while @iter.next
      ScratchPad << @iter.item
    end

    ScratchPad.recorded.should == [1, 2, 3, 4]
  end

  it "raises ObjectBoundsExceededError when the iterator reaches the end" do
    while @iter.next; end
    lambda { @iter.item }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end

describe "Array::Iterator#item for a reverse iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_reverse_iter
  end

  it "raises ObjectBoundsExceededError if the array is empty" do
    lambda { [].to_reverse_iter.item }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "raises ObjectBoundsExceededError before #rnext is called" do
    lambda { @iter.item }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end

  it "returns the item at the current index" do
    while @iter.rnext
      ScratchPad << @iter.item
    end

    ScratchPad.recorded.should == [4, 3, 2, 1]
  end

  it "raises ObjectBoundsExceededError when the iterator reaches the end" do
    while @iter.rnext; end
    lambda { @iter.item }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
