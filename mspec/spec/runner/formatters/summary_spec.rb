require File.dirname(__FILE__) + '/../../spec_helper'
require 'mspec/runner/formatters/summary'
require 'mspec/runner/state'

describe SummaryFormatter, "#after" do
  before :each do
    $stdout = @out = IOStub.new
    @formatter = SummaryFormatter.new
    @state = SpecState.new("describe", "it")
  end

  after :each do
    $stdout = STDOUT
  end

  it "does not print anything" do
    MSpec.stub!(:register)
    tally = mock("tally", :null_object => true)
    tally.stub!(:failures).and_return(1)
    tally.stub!(:errors).and_return(1)
    TallyAction.stub!(:new).and_return(tally)

    @formatter.register
    @state.exceptions << ExpectationNotMetError.new("disappointing")
    @state.exceptions << Exception.new("painful")
    @formatter.after(@state)
    @out.should == ""
  end
end
