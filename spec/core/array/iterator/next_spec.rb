require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator#next for a forward iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_iter
  end

  it "sets the iterator to the start of the array on the first call" do
    @iter.next
    @iter.item.should == 1
    @iter.index.should == 0
    @iter.at(0).should == 1
  end

  it "sets the iterator to the next element on each subsequent call" do
    while @iter.next
      ScratchPad << [@iter.index, @iter.item, @iter.at(0)]
    end

    ScratchPad.recorded.should == [[0, 1, 1], [1, 2, 2], [2, 3, 3], [3, 4, 4]]
  end

  it "returns true if the iterator was advanced" do
    @iter.next.should be_true
  end

  it "returns false if the iterator cannot advance" do
    iter = [1, 2].to_iter
    iter.next.should be_true
    iter.next.should be_true
    iter.next.should be_false
  end

  it "returns false for an empty array" do
    [].to_iter.next.should be_false
  end

  it "returns true once for a one-element array" do
    iter = [1].to_iter
    iter.next.should be_true
    iter.next.should be_false
  end

  it "starts iterating from the array.start sentinel" do
    a = [1, 2, 3, 4, 5]
    a.start = 2
    a.total = 3

    iter = a.to_iter
    while iter.next
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[0, 3], [1, 4], [2, 5]]
  end

  it "advances by an even step increment that evenly divides the array size" do
    a = [1, 2, 3, 4, 5, 6]
    iter = a.to_iter 2

    while iter.next
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[0, 1], [2, 3], [4, 5]]
  end

  it "advances by an even step increment that does not evenly divide the array size" do
    a = [1, 2, 3, 4, 5, 6, 7]
    iter = a.to_iter 2

    while iter.next
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[0, 1], [2, 3], [4, 5], [6,7]]
  end

  it "advances by an odd step increment that evenly divides the array size" do
    a = [1, 2, 3, 4, 5, 6]
    iter = a.to_iter 3

    while iter.next
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[0, 1], [3, 4]]
  end

  it "advances by an odd step increment that does not evenly divide the array size" do
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    iter = a.to_iter 3

    while iter.next
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[0, 1], [3, 4], [6, 7]]
  end
end

describe "Array::Iterator#next for a reverse iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_reverse_iter
  end

  it "returns false for an empty array" do
    [].to_reverse_iter.next.should be_false
  end

  it "returns false when the iterator has not been advanced" do
    @iter.next.should be_false
  end

  it "returns true when the iterator can be advanced forward" do
    @iter.rnext
    @iter.rnext
    @iter.next.should be_true
  end

  it "advances the iterator in the forward direction" do
    while @iter.rnext; end

    while @iter.next
      ScratchPad << @iter.item
    end

    ScratchPad.recorded.should == [1, 2, 3, 4]
  end

  it "starts iterating from the array.start sentinel when #rnext has reached its end" do
    a = [1, 2, 3, 4, 5]
    a.start = 2
    a.total = 3

    iter = a.to_reverse_iter
    while iter.rnext; end

    while iter.next
      ScratchPad << [iter.index, iter.item]
    end

    ScratchPad.recorded.should == [[0, 3], [1, 4], [2, 5]]
  end

  it "moves the iterator forward to positions previously visited by #rnext" do
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    iter = a.to_reverse_iter 3

    iter.rnext
    iter.rnext
    iter.item.should == 5
    iter.index.should == 4

    iter.rnext
    iter.item.should == 2
    iter.index.should == 1

    iter.next
    iter.item.should == 5
    iter.index.should == 4

    iter.next
    iter.item.should == 8
    iter.index.should == 7
  end
end
