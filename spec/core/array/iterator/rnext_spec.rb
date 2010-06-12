require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator#rnext for a reverse iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_reverse_iter
  end

  it "sets the iterator to the end of the array on the first call" do
    @iter.rnext
    @iter.item.should == 4
    @iter.index.should == 3
    @iter.at(0).should == 4
  end

  it "sets the iterator to the previous element on each subsequent call" do
    while @iter.rnext
      ScratchPad << [@iter.index, @iter.item, @iter.at(0)]
    end

    ScratchPad.recorded.should == [[3, 4, 4], [2, 3, 3], [1, 2, 2], [0, 1, 1]]
  end

  it "returns true if the iterator was advanced" do
    @iter.rnext.should be_true
  end

  it "returns false if the iterator cannot advance" do
    iter = [1, 2].to_reverse_iter
    iter.rnext.should be_true
    iter.rnext.should be_true
    iter.rnext.should be_false
  end

  it "returns false for an empty array" do
    [].to_reverse_iter.rnext.should be_false
  end

  it "returns true once for a one-element array" do
    iter = [1].to_reverse_iter
    iter.rnext.should be_true
    iter.rnext.should be_false
  end

  it "stops iterating when it reaches the array.start sentinel" do
    a = [1, 2, 3, 4, 5]
    a.start = 2
    a.total = 3

    iter = a.to_reverse_iter
    while iter.rnext
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[2, 5], [1, 4], [0, 3]]
  end

  it "advances by an even step increment that evenly divides the array size" do
    a = [1, 2, 3, 4, 5, 6]
    iter = a.to_reverse_iter 2

    while iter.rnext
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[5, 6], [3, 4], [1, 2]]
  end

  it "advances by an even step increment that does not evenly divide the array size" do
    a = [1, 2, 3, 4, 5, 6, 7]
    iter = a.to_reverse_iter 2

    while iter.rnext
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[6, 7], [4, 5], [2, 3], [0, 1]]
  end

  it "advances by an odd step increment that evenly divides the array size" do
    a = [1, 2, 3, 4, 5, 6]
    iter = a.to_reverse_iter 3

    while iter.rnext
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[5, 6], [2, 3]]
  end

  it "advances by an odd step increment that does not evenly divide the array size" do
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    iter = a.to_reverse_iter 3

    while iter.rnext
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[7, 8], [4, 5], [1, 2]]
  end
end

describe "Array::Iterator#rnext for a forward iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_iter
  end

  it "returns false for an empty array" do
    [].to_iter.rnext.should be_false
  end

  it "returns false when the iterator has not been advanced by #next" do
    @iter.rnext.should be_false
  end

  it "returns true when the iterator can be advanced forward" do
    @iter.next
    @iter.next
    @iter.rnext.should be_true
  end

  it "advances the iterator in the forward direction" do
    while @iter.next; end

    while @iter.rnext
      ScratchPad << @iter.item
    end

    ScratchPad.recorded.should == [4, 3, 2, 1]
  end

  it "stops iterating when it reaches the array.start sentinel" do
    a = [1, 2, 3, 4, 5]
    a.start = 2
    a.total = 3

    iter = a.to_iter
    while iter.next; end

    while iter.rnext
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[2, 5], [1, 4], [0, 3]]
  end

  it "moves the iterator backward to positions previously visited by #next" do
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    iter = a.to_iter 3

    iter.next
    iter.next
    iter.item.should == 4
    iter.index.should == 3

    iter.next
    iter.item.should == 7
    iter.index.should == 6

    iter.rnext
    iter.item.should == 4
    iter.index.should == 3

    iter.rnext
    iter.item.should == 1
    iter.index.should == 0
  end
end
