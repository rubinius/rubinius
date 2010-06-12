require File.expand_path('../../../../spec_helper', __FILE__)

describe "Array::Iterator.new" do
  before :each do
    ScratchPad.record []
  end

  it "creates a forward iterator" do
    iter = Array::Iterator.new [1, 2, 3], 1, false
    while iter.next
      ScratchPad << iter.item
    end

    ScratchPad.recorded.should == [1, 2, 3]
  end

  it "creates a forward iterator with a step size > 1" do
    iter = Array::Iterator.new [1, 2, 3], 2, false
    while iter.next
      ScratchPad << iter.item
    end

    ScratchPad.recorded.should == [1, 3]
  end

  it "creates a reverse iterator" do
    iter = Array::Iterator.new [1, 2, 3], 1, true
    while iter.rnext
      ScratchPad << iter.item
    end

    ScratchPad.recorded.should == [3, 2, 1]
  end

  it "creates a reverse iterator with a step size > 1" do
    iter = Array::Iterator.new [1, 2, 3], 2, true
    while iter.rnext
      ScratchPad << iter.item
    end

    ScratchPad.recorded.should == [3, 1]
  end
end
