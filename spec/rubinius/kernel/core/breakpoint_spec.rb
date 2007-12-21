require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/breakpoint'

extension :rubinius do
  describe "Breakpoint#initalize" do

    before do
      @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    end

    it "converts a missing instruction pointer argument to 0" do
      bp = Breakpoint.new(@cm, nil) {}
      bp.ip.should == 0
      bp = Breakpoint.new(@cm) {}
      bp.ip.should == 0  
    end

    it "throws an ArgumentError if the IP is out of range" do
      Breakpoint.new(@cm, 10) {}
      lambda { Breakpoint.new(@cm, -1) {} }.should raise_error(ArgumentError)
      lambda { Breakpoint.new(@cm, 1000) {} }.should raise_error(ArgumentError)
      #lambda { Breakpoint.new(@cm, 10) {} }.should_not raise_error(ArgumentError)
    end

    it "throws an ArgumentError if the IP is not the address of an instruction" do
      lambda { Breakpoint.new(@cm, 1) {} }.should raise_error(ArgumentError)
    end

    it "throws an ArgumentError if no block is given" do
      lambda { Breakpoint.new(@cm) }.should raise_error(ArgumentError)
    end

    it "does not modify the compiled method instruction sequence" do
      pre = @cm.bytecodes.decode
      bp = Breakpoint.new(@cm, nil) {}
      bp.enabled?.should == false
      @cm.bytecodes.decode.should == pre
    end
  end

  describe "Breakpoint#enable" do
    before do
      @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    end

    it "inserts a yield_debugger instruction at the location specified by @ip" do
      dc = @cm.bytecodes.decode
      dc[0].first.should_not == :yield_debugger
      bp = Breakpoint.new(@cm) {}
      bp.enable
      dc = @cm.bytecodes.decode
      dc[0].first.should == :yield_debugger

      dc[8].first.should_not == :yield_debugger
      bp = Breakpoint.new(@cm,8) {}
      bp.enable
      dc = @cm.bytecodes.decode
      dc[8].first.should == :yield_debugger
    end

    it "sets the enabled flag to true" do
      bp = Breakpoint.new(@cm,8) {}
      bp.enabled?.should == false
      bp.enable
      bp.enabled?.should == true
    end
  end

  describe "Breakpoint#disable" do
    before do
      @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
      @orig = @cm.bytecodes.decode[8]
      @bp = Breakpoint.new(@cm,8) {}
      @bp.enable
    end

    it "removes the yield_debugger instruction at the location specified by @ip" do
      dc = @cm.bytecodes.decode
      dc[8].first.should == :yield_debugger
      @bp.disable
      dc = @cm.bytecodes.decode
      dc[8].first.should_not == :yield_debugger
      dc[8].should == @orig
    end

    it "sets the enabled flag to false" do
      @bp.enabled?.should == true
      @bp.disable
      @bp.enabled?.should == false
    end
  end

  describe "Breakpoint#call" do
  end
end