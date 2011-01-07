require File.expand_path('../spec_helper', __FILE__)

load_extension('util')

describe "C-API Util function" do
  before :each do
    @o = CApiUtilSpecs.new
  end

  describe "rb_scan_args" do
    it "should assign all required arguments" do
      arguments = [1,2,3]
      ary = @o.rb_scan_args(arguments, "3", 0)
      ary.should == arguments
    end

    it "should assign all required and all passed optional arguments" do
      arguments = [:required, :optional]
      ary = @o.rb_scan_args(arguments, "11", 0)
      ary.should == arguments
      ary = @o.rb_scan_args(arguments, "02", 0)
      ary.should == arguments
      ary = @o.rb_scan_args(arguments, "12", 1)
      ary.should == arguments + [nil]
      ary = @o.rb_scan_args([], "02", 2)
      ary.should == [nil, nil]
    end

    it "should assign all required and all optional arguments and splat the rest" do
      arguments = [:required, :optional, 1, 2]
      ary = @o.rb_scan_args(arguments, "11*", 1)
      ary[0..1].should == arguments[0..1]
      ary[2].should == arguments[2..3]

      arguments = [:required, :optional]
      ary = @o.rb_scan_args(arguments, "11*", 1)
      ary[0..1].should == arguments[0..1]
      ary[2].should == []
    end

    it "should assign all arguments and a possible block" do
      arguments = [:required, :optional, 1, 2]
      block = lambda { 1 }

      ary = @o.rb_scan_args(arguments, "11*&", 2, &block)
      ary[0..1].should == arguments[0..1]
      ary[2].should == arguments[2..3]
      ary[3].should == block

      arguments = [:required, :optional]
      ary = @o.rb_scan_args(arguments, "11*&", 2, &block)
      ary[0..1].should == arguments[0..1]
      ary[2].should == []
      ary[3].should == block

      ary = @o.rb_scan_args(arguments, "11&", 1, &block)
      ary[0..1].should == arguments
      ary[2].should == block

      ary = @o.rb_scan_args(arguments, "11&", 1)
      ary[0..1].should == arguments
      ary[2].should == nil
    end
  end
end
