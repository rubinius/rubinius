require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/specdoc'
require File.dirname(__FILE__) + '/../../../runner/state'

describe SpecdocFormatter, "#after" do
  before :each do
    $stdout = @out = CaptureOutput.new
    @formatter = SpecdocFormatter.new
    @state = SpecState.new("describe", "it")
  end
  
  after :each do
    $stdout = STDOUT
  end
  
  it "prints the #describe string once" do
    @formatter.after(@state)
    @formatter.after(@state)
    @out.should =~ /^describe\n- it\n- it/
  end
  
  it "prints the #it once when there are no exceptions raised" do
    @formatter.after(@state)
    @out.should =~ /^describe\n- it\n$/
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
    @out.should == "describe\n- it (FAILED - 1)\n- it (ERROR - 2)\n"
  end
end  
