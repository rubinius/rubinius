require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator#index for a forward iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_iter
  end

  it "returns -step before #next is called" do
    @iter.index.should == -1
    [1, 2, 3].to_iter(2).index.should == -2
  end

  it "returns -step before #next is called for an array with start > 0" do
    a = [1, 2, 3, 4, 5]
    a.start = 2
    a.total = 3

    iter = a.to_iter
    iter.index.should == -1
  end

  it "returns the index of the current item" do
    while @iter.next
      ScratchPad << @iter.index
    end

    ScratchPad.recorded.should == [0, 1, 2, 3]
  end

  it "returns the last index + step when the iterator reaches the end" do
    while @iter.next; end
    @iter.index.should == 4
    @iter.index.should == 4
  end
end

describe "Array::Iterator#index for a reverse iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_reverse_iter
  end

  it "returns the last index + step before #rnext is called" do
    @iter.index.should == 4
    [1, 2, 3].to_reverse_iter(2).index.should == 4
  end

  it "returns the index of the current item" do
    while @iter.rnext
      ScratchPad << @iter.index
    end

    ScratchPad.recorded.should == [3, 2, 1, 0]
  end

  it "returns the first index - step when the iterator reaches the end" do
    while @iter.rnext; end
    @iter.index.should == -1
    @iter.index.should == -1

    iter = [1, 2, 3].to_reverse_iter(2)
    while iter.rnext; end
    iter.index.should == -2
    iter.index.should == -2
  end
end
