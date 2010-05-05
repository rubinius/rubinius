require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Options#parse" do
  before :each do
    @opt = Rubinius::Options.new
    @prc = lambda { ScratchPad.record :parsed }
    @arg_prc = lambda { |o| ScratchPad.record [:parsed, o] }
    ScratchPad.clear
  end

  it "parses a short option" do
    @opt.on "-a", "desc", &@prc
    @opt.parse ["-a"]
    ScratchPad.recorded.should == :parsed
  end

  it "parse '-' as an option" do
    @opt.on "-", "desc", &@prc
    @opt.parse ["-"]
    ScratchPad.recorded.should == :parsed
  end

  it "parse a long option" do
    @opt.on "--abdc", "desc", &@prc
    @opt.parse ["--abdc"]
    ScratchPad.recorded.should == :parsed
  end

  it "parses '--' as an option" do
    @opt.on "--", "desc", &@prc
    @opt.parse ["--"]
    ScratchPad.recorded.should == :parsed
  end

  it "parses a short option group" do
    @opt.on "-a", "ARG", "desc", &@arg_prc
    @opt.parse ["-a", "ARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a short option with an argument" do
    @opt.on "-a", "ARG", "desc", &@arg_prc
    @opt.parse ["-a", "ARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a short option with connected argument" do
    @opt.on "-a", "ARG", "desc", &@arg_prc
    @opt.parse ["-aARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a short option with optional argument instead of option group" do
    ScratchPad.record []
    @opt.on("-a", "[ARG]", "desc") { |o| ScratchPad << [:a, o] }
    @opt.on("-b", "desc") { |o| ScratchPad << :b }
    @opt.parse ["-ab"]
    ScratchPad.recorded.should == [[:a, "b"]]
  end

  it "parses a short option group with optional argument without consuming the next entry" do
    ScratchPad.record []
    @opt.on("-a", "[ARG]", "desc") { |o| ScratchPad << [:a, o] }
    @opt.on("-b", "desc") { |o| ScratchPad << :b }
    @opt.parse(["-ba", "ARG"]).should == ["ARG"]
    ScratchPad.recorded.should == [:b, [:a, nil]]
  end

  it "parses a short option group with attached optional argument" do
    ScratchPad.record []
    @opt.on("-a", "[ARG]", "desc") { |o| ScratchPad << [:a, o] }
    @opt.on("-b", "desc") { |o| ScratchPad << :b }
    @opt.parse ["-baARG"]
    ScratchPad.recorded.should == [:b, [:a, "ARG"]]
  end

  it "parses a short option with an optional argument missing" do
    @opt.on "-a", "[ARG]", "desc", &@arg_prc
    @opt.parse ["-a"]
    ScratchPad.recorded.should == [:parsed, nil]
  end

  it "parses a short option with optional argument followed by an option" do
    ScratchPad.record []
    @opt.on("-a", "[ARG]", "desc") { |o| ScratchPad << [:a, o] }
    @opt.on("-b", "desc") { |o| ScratchPad << :b }
    @opt.parse ["-a", "-b"]
    ScratchPad.recorded.should == [[:a, nil], :b]
  end

  it "parses a short option with an optional argument without consuming the next entry" do
    @opt.on "-a", "[ARG]", "desc", &@arg_prc
    @opt.parse(["-a", "ARG"]).should == ["ARG"]
    ScratchPad.recorded.should == [:parsed, nil]
  end

  it "parses a short option with an attached optional argument" do
    @opt.on "-a", "[ARG]", "desc", &@arg_prc
    @opt.parse ["-aARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a long option with an argument" do
    @opt.on "--abdc", "ARG", "desc", &@arg_prc
    @opt.parse ["--abdc", "ARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a long option with an optional argument without consuming the next entry" do
    @opt.on "--abdc", "[ARG]", "desc", &@arg_prc
    @opt.parse(["--abdc", "ARG"]).should == ["ARG"]
    ScratchPad.recorded.should == [:parsed, nil]
  end

  it "parses a long option with an optional '=' argument" do
    @opt.on "--abdc", "[ARG]", "desc", &@arg_prc
    @opt.parse ["--abdc=ARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a long option with an optional argument missing" do
    @opt.on "--abdc", "[ARG]", "desc", &@arg_prc
    @opt.parse ["--abdc"]
    ScratchPad.recorded.should == [:parsed, nil]
  end

  it "parses a long option with an '=' argument" do
    @opt.on "--abdc", "ARG", "desc", &@arg_prc
    @opt.parse ["--abdc=ARG"]
    ScratchPad.recorded.should == [:parsed, "ARG"]
  end

  it "parses a short option group with the final option taking an argument" do
    ScratchPad.record []
    @opt.on("-a", "desc") { |o| ScratchPad << :a }
    @opt.on("-b", "ARG", "desc") { |o| ScratchPad << [:b, o] }
    @opt.parse ["-ab", "ARG"]
    ScratchPad.recorded.should == [:a, [:b, "ARG"]]
  end

  it "parses a short option group with a connected argument" do
    ScratchPad.record []
    @opt.on("-a", "desc") { |o| ScratchPad << :a }
    @opt.on("-b", "ARG", "desc") { |o| ScratchPad << [:b, o] }
    @opt.on("-c", "desc") { |o| ScratchPad << :c }
    @opt.parse ["-acbARG"]
    ScratchPad.recorded.should == [:a, :c, [:b, "ARG"]]
  end

  it "returns the unprocessed entries" do
    @opt.on "-a", "ARG", "desc", &@arg_prc
    @opt.parse(["abdc", "-a", "ilny"]).should == ["abdc"]
  end

  it "calls the on_extra handler with unrecognized options" do
    ScratchPad.record []
    @opt.on_extra { |o| ScratchPad << o }
    @opt.on "-a", "desc"
    @opt.parse ["-a", "-b"]
    ScratchPad.recorded.should == ["-b"]
  end

  it "does not attempt to call the block if it is nil" do
    @opt.on "-a", "ARG", "desc"
    @opt.parse(["-a", "ARG"]).should == []
  end

  it "raises Rubinius::Options::ParseError if passed an unrecognized option" do
    @opt.should_receive(:raise).with(Rubinius::Options::ParseError, "Unrecognized option: -u")
    @opt.stub!(:puts)
    @opt.stub!(:exit)
    @opt.parse "-u"
  end
end
