require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/actions/tag'
require File.dirname(__FILE__) + '/../../../runner/runner'
require File.dirname(__FILE__) + '/../../../runner/state'
require File.dirname(__FILE__) + '/../../../runner/tag'

describe TagAction do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
  end
  
  it "creates an MatchFilter with its tag and desc arguments" do
    filter = mock('action filter', :null_object => true)
    MatchFilter.should_receive(:new).with(nil, "some", "thing").and_return(filter)
    TagAction.new :all, nil, nil, ["tag", "key"], ["some", "thing"]
  end
end

describe TagAction, "#outcome?" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @state = SpecState.new "describe", "it"
    @exception = [nil, Exception.new("failed")]
  end

  it "returns true if outcome is :fail and the spec fails" do
    action = TagAction.new :fail, nil, nil, nil, nil
    @state.exceptions << @exception
    action.outcome?(@state).should == true
  end
  
  it "returns false if the outcome is :fail and the spec passes" do
    action = TagAction.new :fail, nil, nil, nil, nil
    action.outcome?(@state).should == false
  end
  
  it "returns true if the outcome is :pass and the spec passes" do
    action = TagAction.new :pass, nil, nil, nil, nil
    action.outcome?(@state).should == true
  end
  
  it "returns false if the outcome is :pass and the spec fails" do
    action = TagAction.new :pass, nil, nil, nil, nil
    @state.exceptions << @exception
    action.outcome?(@state).should == false
  end
  
  it "returns true if the outcome is :all" do
    action = TagAction.new :all, nil, nil, nil, nil
    @state.exceptions << @exception
    action.outcome?(@state).should == true
  end
end

describe TagAction, "#after" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @state = SpecState.new "Catch#me", "if you can"
    @tag = SpecTag.new "tag(comment):Catch#me if you can"
    SpecTag.stub!(:new).and_return(@tag)
    @exception = [nil, Exception.new("failed")]
  end
  
  it "does not write a tag if the description does not match" do
    MSpec.should_not_receive(:write_tag)
    action = TagAction.new :all, "tag", "comment", nil, "match"
    action.after @state
  end
  
  it "does not write a tag if outcome is :fail and the spec passed" do
    MSpec.should_not_receive(:write_tag)
    action = TagAction.new :fail, "tag", "comment", nil, "can"
    action.after @state
  end
  
  it "writes a tag if the outcome is :fail and the spec failed" do
    MSpec.should_receive(:write_tag).with(@tag)
    action = TagAction.new :fail, "tag", "comment", nil, "can"
    @state.exceptions << @exception
    action.after @state
  end
  
  it "does not write a tag if outcome is :pass and the spec failed" do
    MSpec.should_not_receive(:write_tag)
    action = TagAction.new :pass, "tag", "comment", nil, "can"
    @state.exceptions << @exception
    action.after @state
  end
  
  it "writes a tag if the outcome is :pass and the spec passed" do
    MSpec.should_receive(:write_tag).with(@tag)
    action = TagAction.new :pass, "tag", "comment", nil, "can"
    action.after @state
  end
  
  it "writes a tag if the outcome is :all" do
    MSpec.should_receive(:write_tag).with(@tag)
    action = TagAction.new :all, "tag", "comment", nil, "can"
    action.after @state
  end
end

describe TagAction, "#register" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @action = TagAction.new :all, nil, nil, nil, nil
  end
  
  it "registers itself with MSpec for the :after action" do
    MSpec.should_receive(:register).with(:after, @action)
    @action.register
  end
end

describe TagAction, "#unregister" do
  before :each do
    MSpec.stub!(:read_tags).and_return([])
    @action = TagAction.new :all, nil, nil, nil, nil
  end
  
  it "unregisters itself with MSpec for the :after action" do
    MSpec.should_receive(:unregister).with(:after, @action)
    @action.unregister
  end
end
