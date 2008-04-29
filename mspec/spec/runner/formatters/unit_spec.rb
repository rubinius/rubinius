require File.dirname(__FILE__) + '/../../spec_helper'
require 'mspec/runner/formatters/unit'
require 'mspec/runner/state'

describe UnitdiffFormatter, "#finish" do
  before :each do
    @tally = mock("tally", :null_object => true)
    TallyAction.stub!(:new).and_return(@tally)
    @timer = mock("timer", :null_object => true)
    TimerAction.stub!(:new).and_return(@timer)

    $stdout = @out = IOStub.new
    @state = SpecState.new("describe", "it")
    MSpec.stub!(:register)
    @formatter = UnitdiffFormatter.new
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

Finished in 2.0 seconds

1)
describe it ERROR
Exception occurred during: msg
broken: 
path/to/some/file.rb:35:in method

1 example, 0 failures
]
  end
end
