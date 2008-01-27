require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../mspec'
require File.dirname(__FILE__) + '/../../runner/state'

describe RunState do
  before :each do
    @state = RunState.new
    @proc = lambda { }
  end
  
  it "records before(:all) blocks" do
    @state.before(:all, &@proc)
    @state.instance_variable_get(:@start).should == [@proc]
  end
  
  it "records before(:each) blocks" do
    @state.before(:each, &@proc)
    @state.instance_variable_get(:@before).should == [@proc]
  end
  
  it "records after(:all) blocks" do
    @state.after(:all, &@proc)
    @state.instance_variable_get(:@finish).should == [@proc]
  end
  
  it "records after(:each) blocks" do
    @state.after(:each, &@proc)
    @state.instance_variable_get(:@after).should == [@proc]
  end
  
  it "records it blocks" do
    @state.it("message", &@proc)
    @state.instance_variable_get(:@spec).should == [["message", @proc]]
  end
  
  it "records describe blocks" do
    @state.describe(Object, "message", &@proc)
    @state.instance_variable_get(:@describe).should == "Object message"
    @state.instance_variable_get(:@block).should == @proc
  end
end

describe RunState, "#protect" do
  it "calls MSpec.protect" do
    record = []
    a = lambda { record << :a }
    b = lambda { record << :b }
    RunState.new.protect("message", [a, b])
    record.should == [:a, :b]
  end
end

describe RunState, "#process" do
  before :all do
    module Mock; end
  end
  
  before :each do
    @state = RunState.new
    @a = lambda { @record << :a }
    @b = lambda { @record << :b }
    @record = []
  end
  
  it "calls each before(:all) block" do
    @state.before(:all, &@a)
    @state.before(:all, &@b)
    @state.process
    @record.should == [:a, :b]
  end
  
  it "calls each after(:all) block" do
    @state.after(:all, &@a)
    @state.after(:all, &@b)
    @state.process
    @record.should == [:a, :b]
  end
  
  it "calls each it block" do
    @state.it("one", &@a)
    @state.it("two", &@b)
    @state.process
    @record.should == [:a, :b]
  end
  
  it "calls each before(:each) block" do
    @state.before(:each, &@a)
    @state.before(:each, &@b)
    @state.it("") { }
    @state.process
    @record.should == [:a, :b]
  end
  
  it "calls each after(:each) block" do
    @state.after(:each, &@a)
    @state.after(:each, &@b)
    @state.it("") { }
    @state.process
    @record.should == [:a, :b]
  end
  
  it "calls Mock.cleanup for each it block" do
    @state.it("") { }
    @state.it("") { }
    Mock.should_receive(:cleanup).twice
    @state.process
  end
  
  it "calls the describe block" do
    record = []
    @state.describe(Object, "msg") { record << :a }
    @state.process
    record.should == [:a]
  end
end
