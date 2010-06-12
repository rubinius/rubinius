require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#stop_parsing" do
  before :each do
    @opt = Rubinius::Options.new
    @prc = lambda { |o| ScratchPad.record [:parsed, o] }
    ScratchPad.clear
  end

  it "stops option parsing" do
    @opt.on "-a", "ARG", "desc", &@prc
    @opt.on "-b", "BRG", "desc", &@prc

    options = @opt
    @opt.on("--", "desc") { options.stop_parsing }

    argv = ["-a", "ARG", "--", "-b"]
    @opt.parse argv
    argv.should == ["-b"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end
end
