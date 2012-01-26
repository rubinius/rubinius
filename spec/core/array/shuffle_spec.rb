require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#shuffle!" do
  it "uses correct indexes after a #shift" do
    # https://github.com/rubinius/rubinius/pull/1527
    # This spec obviously is tied to the current implementation of
    # Array#shuffle! using Kernel.rand

    Kernel.stub!(:rand).and_return(1, 0)

    ary = 1, 2, 3
    ary.shift

    ary.shuffle!.should == [3, 2]
  end
end
