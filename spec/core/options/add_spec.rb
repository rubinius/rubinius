require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#add" do
  before :each do
    @opt = Rubinius::Options.new "cmd", 20
    @prc = lambda { }
  end

  it "adds documentation for an option" do
    @opt.should_receive(:doc).with("   -t, --typo ARG   Correct typo ARG")
    @opt.add("-t", "--typo", "ARG", "Correct typo ARG", @prc)
  end

  it "leaves spaces in the documentation for a missing short option" do
    @opt.should_receive(:doc).with("       --typo ARG   Correct typo ARG")
    @opt.add(nil, "--typo", "ARG", "Correct typo ARG", @prc)
  end

  it "handles a short option with argument but no long argument" do
    @opt.should_receive(:doc).with("   -t ARG           Correct typo ARG")
    @opt.add("-t", nil, "ARG", "Correct typo ARG", @prc)
  end

  it "registers an option" do
    option = Rubinius::Options::Option.new "-t", "--typo", "ARG", "Correct typo ARG", @prc
    Rubinius::Options::Option.should_receive(:new).with(
        "-t", "--typo", "ARG", "Correct typo ARG", @prc).and_return(option)
    @opt.add("-t", "--typo", "ARG", "Correct typo ARG", @prc)
    @opt.options.should == [option]
  end
end
