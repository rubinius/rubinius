require File.dirname(__FILE__) + '/../../spec_helper'
require 'mspec/runner/formatters/dotted'
require 'mspec/runner/mspec'
require 'mspec/runner/state'

describe DottedFormatter, "#initialize" do
  it "permits zero arguments" do
    DottedFormatter.new
  end

  it "accepts one argument" do
    DottedFormatter.new nil
  end
end

describe DottedFormatter, "#register" do
  before :each do
    @formatter = DottedFormatter.new
  end

  it "registers self with MSpec for appropriate actions" do
    MSpec.stub!(:register)
    MSpec.should_receive(:register).with(:after, @formatter)
    MSpec.should_receive(:register).with(:finish, @formatter)
    @formatter.register
  end

  it "creates TimerAction and TallyAction" do
    timer = mock("timer")
    tally = mock("tally")
    timer.should_receive(:register)
    tally.should_receive(:register)
    tally.should_receive(:counter)
    TimerAction.should_receive(:new).and_return(timer)
    TallyAction.should_receive(:new).and_return(tally)
    @formatter.register
  end
end

describe DottedFormatter, "#print" do
  after :each do
    $stdout = STDOUT
  end

  it "writes to $stdout by default" do
    $stdout = IOStub.new
    formatter = DottedFormatter.new
    formatter.print "begonias"
    $stdout.should == "begonias"
  end

  it "writes to the file specified when the formatter was created" do
    out = IOStub.new
    File.should_receive(:open).with("some/file", "w").and_return(out)
    formatter = DottedFormatter.new "some/file"
    formatter.print "begonias"
    out.should == "begonias"
  end
end

describe DottedFormatter, "#after" do
  before :each do
    $stdout = @out = IOStub.new
    @formatter = DottedFormatter.new
    @state = SpecState.new("describe", "it")
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints a '.' if there was no exception raised" do
    @formatter.after(@state)
    @out.should == "."
  end

  it "prints an 'F' if there was an expectation failure" do
    @state.exceptions << ["msg", ExpectationNotMetError.new("failed")]
    @formatter.after(@state)
    @out.should == "F"
  end

  it "prints an 'E' if there was an exception other than expectation failure" do
    @state.exceptions << ["msg", Exception.new("boom!")]
    @formatter.after(@state)
    @out.should == "E"
  end

  it "prints an 'E' if there are mixed exceptions and exepctation failures" do
    @state.exceptions << ["msg", ExpectationNotMetError.new("failed")]
    @state.exceptions << ["msg", Exception.new("boom!")]
    @formatter.after(@state)
    @out.should == "E"
  end
end

describe DottedFormatter, "#finish" do
  before :each do
    @tally = mock("tally", :null_object => true)
    TallyAction.stub!(:new).and_return(@tally)
    @timer = mock("timer", :null_object => true)
    TimerAction.stub!(:new).and_return(@timer)

    $stdout = @out = IOStub.new
    @state = SpecState.new("describe", "it")
    MSpec.stub!(:register)
    @formatter = DottedFormatter.new
    @formatter.register
  end

  after :each do
    $stdout = STDOUT
  end

  it "prints a failure message for an exception" do
    @state.exceptions << ["msg", Exception.new("broken")]
    @formatter.after @state
    @formatter.finish
    @out.should =~ /^1\)\ndescribe it ERROR$/
  end

  it "prints a backtrace for an exception" do
    @formatter.stub!(:backtrace).and_return("path/to/some/file.rb:35:in method")
    @state.exceptions << ["msg", Exception.new("broken")]
    @formatter.after @state
    @formatter.finish
    @out.should =~ %r[path/to/some/file.rb:35:in method$]
  end

  it "prints a summary of elapsed time" do
    @timer.should_receive(:format).and_return("Finished in 2.0 seconds")
    @formatter.finish
    @out.should =~ /^Finished in 2.0 seconds$/
  end

  it "prints a tally of counts" do
    @tally.should_receive(:format).and_return("1 example, 0 failures")
    @formatter.finish
    @out.should =~ /^1 example, 0 failures$/
  end

  it "prints errors, backtraces, elapsed time, and tallies" do
    @state.exceptions << ["msg", Exception.new("broken")]
    @formatter.stub!(:backtrace).and_return("path/to/some/file.rb:35:in method")
    @timer.should_receive(:format).and_return("Finished in 2.0 seconds")
    @tally.should_receive(:format).and_return("1 example, 0 failures")
    @formatter.after @state
    @formatter.finish
    @out.should ==
%[E

1)
describe it ERROR
Exception occurred during: msg
broken
path/to/some/file.rb:35:in method

Finished in 2.0 seconds

1 example, 0 failures
]
  end
end
