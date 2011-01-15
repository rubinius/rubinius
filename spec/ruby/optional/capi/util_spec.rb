require File.expand_path('../spec_helper', __FILE__)

load_extension('util')

describe "C-API Util function" do
  before :each do
    @o = CApiUtilSpecs.new
  end

  describe "rb_scan_args" do
    before :each do
      @prc = lambda { 1 }
      @acc = []
      ScratchPad.record @acc
    end

    it "assigns the required arguments scanned" do
      @o.rb_scan_args([1, 2], "2", 2, @acc).should == 2
      ScratchPad.recorded.should == [1, 2]
    end

    it "raises an ArgumentError if there are insufficient arguments" do
      lambda { @o.rb_scan_args([1, 2], "3", 0, @acc) }.should raise_error(ArgumentError)
    end

    it "assigns the required and optional arguments scanned" do
      @o.rb_scan_args([1, 2], "11", 2, @acc).should == 2
      ScratchPad.recorded.should == [1, 2]
    end

    it "assigns the optional arguments scanned" do
      @o.rb_scan_args([1, 2], "02", 2, @acc).should == 2
      ScratchPad.recorded.should == [1, 2]
    end

    it "assigns nil for optional arguments that are not present" do
      @o.rb_scan_args([1], "03", 3, @acc).should == 1
      ScratchPad.recorded.should == [1, nil, nil]
    end

    it "assigns the required and optional arguments and splats the rest" do
      @o.rb_scan_args([1, 2, 3, 4], "11*", 3, @acc).should == 4
      ScratchPad.recorded.should == [1, 2, [3, 4]]
    end

    it "assigns the required and optional arguments and and empty Array when there are no arguments to splat" do
      @o.rb_scan_args([1, 2], "11*", 3, @acc).should == 2
      ScratchPad.recorded.should == [1, 2, []]
    end

    it "assigns required, optional arguments scanned and the passed block" do
      @o.rb_scan_args([1, 2], "11&", 3, @acc, &@prc).should == 2
      ScratchPad.recorded.should == [1, 2, @prc]
    end

    it "assigns required, optional, splatted arguments scanned and the passed block" do
      @o.rb_scan_args([1, 2, 3, 4], "11*&", 4, @acc, &@prc).should == 4
      ScratchPad.recorded.should == [1, 2, [3, 4], @prc]
    end

    it "assigns required arguments, nil for missing optional arguments and the passed block" do
      @o.rb_scan_args([1], "12&", 4, @acc, &@prc).should == 1
      ScratchPad.recorded.should == [1, nil, nil, @prc]
    end

    it "assigns required, splatted arguments and the passed block" do
      @o.rb_scan_args([1, 2, 3], "1*&", 3, @acc, &@prc).should == 3
      ScratchPad.recorded.should == [1, [2, 3], @prc]
    end
  end
end
