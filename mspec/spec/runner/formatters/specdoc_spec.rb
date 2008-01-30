require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/specdoc'
require File.dirname(__FILE__) + '/../../../runner/state'

describe SpecdocFormatter do
  before :each do
    @formatter = SpecdocFormatter.new
  end
  
  it "responds to #register by registering itself with MSpec for appropriate actions" do
    MSpec.stub!(:register)
    MSpec.should_receive(:register).with(:enter, @formatter)
    @formatter.register
  end
end

describe SpecdocFormatter, "#enter" do
  before :each do
    $stdout = @out = CaptureOutput.new
    @formatter = SpecdocFormatter.new
  end
  
  after :each do
    $stdout = STDOUT
  end
  
  it "prints the #describe string" do
    @formatter.enter("describe")
    @out.should == "describe\n"
  end
end

describe SpecdocFormatter, "#after" do
  before :each do
    $stdout = @out = CaptureOutput.new
    @formatter = SpecdocFormatter.new
    @state = SpecState.new("describe", "it")
  end
  
  after :each do
    $stdout = STDOUT
  end
  
  it "prints the #it once when there are no exceptions raised" do
    @formatter.after(@state)
    @out.should == "- it\n"
  end
  
  it "prints the #it string once for each exception raised" do
    MSpec.stub!(:register)
    tally = mock("tally", :null_object => true)
    tally.stub!(:failures).and_return(1)
    tally.stub!(:errors).and_return(1)
    TallyAction.stub!(:new).and_return(tally)
    
    @formatter.register
    @state.exceptions << ExpectationNotMetError.new("disappointing")
    @state.exceptions << Exception.new("painful")
    @formatter.after(@state)
    @out.should == "- it (FAILED - 1)\n- it (ERROR - 2)\n"
  end
end  
