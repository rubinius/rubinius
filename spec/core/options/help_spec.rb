require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#help" do
  before :each do
    @opt = Rubinius::Options.new
    ScratchPad.clear
  end

  it "installs a basic -h, --help option" do
    @opt.should_receive(:puts)
    @opt.should_receive(:exit).with(1)
    @opt.help
    @opt.parse "-h"
  end

  it "accepts a block instead of using the default block" do
    @opt.help { |o| ScratchPad.record :help }
    @opt.parse "-h"
    ScratchPad.recorded.should == :help
  end
end
