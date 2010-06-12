require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#process" do
  before :each do
    @opt = Rubinius::Options.new
    ScratchPad.clear
  end

  it "calls the on_extra block if the argument does not match any option" do
    @opt.on_extra { ScratchPad.record :extra }
    @opt.process ["-a"], "-a", "-a", nil
    ScratchPad.recorded.should == :extra
  end

  it "returns the matching option" do
    @opt.on "-a", "ARG", "desc"
    option = @opt.process [], "-a", "-a", "ARG"
    option.should be_kind_of(Rubinius::Options::Option)
    option.short.should == "-a"
    option.arg.should == "ARG"
    option.description.should == "desc"
  end

  it "raises an Rubinius::Options::ParseError if arg is nil and there are no more entries in argv" do
    @opt.on "-a", "ARG", "desc"
    lambda { @opt.process [], "-a", "-a", nil }.should raise_error(Rubinius::Options::ParseError)
  end

  it "fetches the argument for the option from argv if arg is nil" do
    @opt.on("-a", "ARG", "desc") { |o| ScratchPad.record o }
    @opt.process ["ARG"], "-a", "-a", nil
    ScratchPad.recorded.should == "ARG"
  end

  it "calls the option's block" do
    @opt.on("-a", "ARG", "desc") { ScratchPad.record :option }
    @opt.process [], "-a", "-a", "ARG"
    ScratchPad.recorded.should == :option
  end

  it "does not call the option's block if it is nil" do
    @opt.on "-a", "ARG", "desc"
    lambda { @opt.process [], "-a", "-a", "ARG" }.should_not raise_error
  end

  it "ignores post equal text when considering an optional arg" do
    enabled = nil
    prefix = nil

    @opt.on("--en", "[OPT]", "desc") { |e| enabled = e || :def }
    @opt.on("--prefix", "OPT", "desc") { |x| prefix = x }
    @opt.process ["--prefix=blah"], "--en", "--en", nil

    enabled.should == :def

  end
end
