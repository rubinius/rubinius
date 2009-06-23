require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Options#version" do
  before :each do
    @opt = Rubinius::Options.new
    ScratchPad.clear
  end

  it "installs a basic -v, --version option" do
    @opt.should_receive(:puts)
    @opt.should_receive(:exit)
    @opt.version "1.0.0"
    @opt.parse "-v"
  end

  it "accepts a block instead of using the default block" do
    @opt.version("1.0.0") { |o| ScratchPad.record :version }
    @opt.parse "-v"
    ScratchPad.recorded.should == :version
  end
end
