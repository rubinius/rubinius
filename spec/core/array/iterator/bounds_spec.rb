require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator#bounds for a forward iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_iter
  end

  it "returns self" do
    @iter.bounds!(2, 3).should equal(@iter)
  end

  it "can set the bounds so only one element is available" do
    @iter.bounds! 2, 3
    while @iter.next
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[2, 3]]
  end

  it "can set the bounds so that no element is available" do
    @iter.bounds! 3, 2
    while @iter.next
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == []
  end

  it "sets the start and stop boundaries" do
    @iter.bounds! 1, 3
    while @iter.next
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[1, 2], [2, 3]]
  end

  it "constrains the start value to the beginning of the array" do
    @iter.bounds! -3, 4
    while @iter.next
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[0, 1], [1, 2], [2, 3], [3, 4]]
  end

  it "constrains the stop value to the end of the array" do
    @iter.bounds! 0, 5
    while @iter.next
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[0, 1], [1, 2], [2, 3], [3, 4]]
  end
end

describe "Array::Iterator#bounds for a reverse iterator" do
  before :each do
    ScratchPad.record []

    @iter = [1, 2, 3, 4].to_reverse_iter
  end

  it "returns self" do
    @iter.bounds!(2, 3).should equal(@iter)
  end

  it "can set the bounds so only one element is available" do
    @iter.bounds! 1, 2
    while @iter.rnext
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[1, 2]]
  end

  it "can set the bounds so that no element is available" do
    @iter.bounds! 3, 2
    while @iter.rnext
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == []
  end

  it "sets the start and stop boundaries" do
    @iter.bounds! 1, 3
    while @iter.rnext
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[2, 3], [1, 2]]
  end

  it "constrains the stop value to the beginning of the array" do
    @iter.bounds! -3, 4
    while @iter.rnext
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[3, 4], [2, 3], [1, 2], [0, 1]]
  end

  it "constrains the start value to the end of the array" do
    @iter.bounds! 0, 5
    while @iter.rnext
      ScratchPad << [@iter.index, @iter.item]
    end

    ScratchPad.recorded.should == [[3, 4], [2, 3], [1, 2], [0, 1]]
  end
end
