require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/spinner'
require File.dirname(__FILE__) + '/../../../runner/mspec'
require File.dirname(__FILE__) + '/../../../runner/state'

describe SpinnerFormatter do
  before :each do
    $stdout = @out = CaptureOutput.new
    @formatter = SpinnerFormatter.new
  end

  after :each do
    $stdout = STDOUT
  end

  it "responds to #register by registering itself with MSpec for appropriate actions" do
    MSpec.stub!(:register)
    MSpec.should_receive(:register).with(:start, @formatter)
    MSpec.should_receive(:register).with(:load, @formatter)
    MSpec.should_receive(:register).with(:after, @formatter)
    MSpec.should_receive(:register).with(:finish, @formatter)
    @formatter.register
  end

  it "responds to #register by creating TimerAction and TallyAction" do
    timer = mock("timer")
    tally = mock("tally")
    timer.should_receive(:register)
    tally.should_receive(:register)
    TimerAction.should_receive(:new).and_return(timer)
    TallyAction.should_receive(:new).and_return(tally)
    @formatter.register
  end
end

describe SpinnerFormatter, "#after" do
  before :each do
    @formatter = SpinnerFormatter.new
    @state = SpecState.new("describe", "it")
  end

  it "updates the spinner" do
    @formatter.should_receive(:spin)
    @formatter.after @state
  end
end
