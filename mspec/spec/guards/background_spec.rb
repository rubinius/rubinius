require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/background'

describe Object, "#process_is_foreground" do
  before :each do
    MSpec.clear_modes
    ScratchPad.clear
  end

  it "yields if MSpec.mode?(:background) is false" do
    MSpec.mode?(:background).should be_false
    process_is_foreground { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end

  it "does not yield if MSpec.mode?(:background) is true" do
    MSpec.register_mode :background
    process_is_foreground { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end
end
