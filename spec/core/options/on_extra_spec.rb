require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#on_extra" do
  before :each do
    @opt = Rubinius::Options.new
    ScratchPad.clear
  end

  it "registers a block to be called when an option is not recognized" do
    @opt.on_extra { ScratchPad.record :extra }
    @opt.parse "-g"
    ScratchPad.recorded.should == :extra
  end
end
