require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../expectations'
require File.dirname(__FILE__) + '/../../../runner/actions/tally'
require File.dirname(__FILE__) + '/../../../runner/mspec'
require File.dirname(__FILE__) + '/../../../runner/state'

describe TallyAction do
  before :each do
    @tally = TallyAction.new
    @state = SpecState.new("describe", "it")
    @state.exceptions << ["msg", Exception.new("it broke")]
    @state.exceptions << ["msg", ExpectationNotMetError.new("disappointment")]
  end
  
  it "responds to #load by incrementing the file count" do
    @tally.load
    @tally.files.should == 1
  end
  
  it "responds to #expecctations by incrementing the expectations count" do
    @tally.expectation @state
    @tally.expectations.should == 1
  end
  
  it "responds to #after by incrementing examples, failures, errors counts" do
    @tally.after @state
    @tally.examples.should == 1
    @tally.failures.should == 1
    @tally.errors.should == 1
  end
  
  it "responds to #format by returning a readable string of counts" do
    @tally.load
    @tally.after @state
    @tally.expectation @state
    @tally.expectation @state
    @tally.format.should == "1 file, 1 example, 2 expectations, 1 failure, 1 error"
  end
  
  it "responds to #register by registering itself with MSpec for appropriate actions" do
    MSpec.should_receive(:register).with(:load, @tally)
    MSpec.should_receive(:register).with(:after, @tally)
    MSpec.should_receive(:register).with(:expectation, @tally)
    @tally.register
  end
  
  it "responds to #unregister by unregistering itself with MSpec for appropriate actions" do
    MSpec.should_receive(:unregister).with(:load, @tally)
    MSpec.should_receive(:unregister).with(:after, @tally)
    MSpec.should_receive(:unregister).with(:expectation, @tally)
    @tally.unregister
  end
end
