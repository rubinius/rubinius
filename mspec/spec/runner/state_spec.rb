require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../mspec'
require File.dirname(__FILE__) + '/../../runner/runner'
require File.dirname(__FILE__) + '/../../mocks/mock'
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

describe RunState, "#state" do
  before :each do
    @state = RunState.new
  end
  
  it "returns nil if no spec is being executed" do
    @state.state.should == nil
  end
  
  it "returns a SpecState instance if a spec is being executed" do
    record = nil
    @state.describe("") { }
    @state.it("") { record = @state.state }
    @state.process
    @state.state.should == nil
    record.should be_kind_of(SpecState)
  end
end

describe RunState, "#process" do  
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
  
  it "creates a new SpecState instance for each spec" do
    SpecState.should_receive(:new).with("describe", "it")
    @state.describe("describe") { }
    @state.it("it") { }
    @state.process
  end
  
  it "records exceptions that occur while running the spec" do
    record = nil
    exception = Exception.new("bump!")
    MSpec.stack.push @state
    @state.describe("describe") { }
    @state.it("it") { raise exception }
    @state.after(:each) { record = @state.state.exceptions }
    @state.process
    record.should == [[nil, exception]]
  end
end

describe RunState, "#process" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []
    
    @state = RunState.new
    @state.describe("") { }
    @state.it("") { }
  end

  it "calls registered before actions with the current SpecState instance" do
    before = mock("before")
    before.should_receive(:before).and_return { 
      @record = :before
      @spec_state = @state.state
    }
    MSpec.register :before, before
    @state.process
    @record.should == :before
    @spec_state.should be_kind_of(SpecState)
  end

  it "calls registered after actions with the current SpecState instance" do
    after = mock("after")
    after.should_receive(:after).and_return {
      @record = :after
      @spec_state = @state.state
    }
    MSpec.register :after, after
    @state.process
    @record.should == :after
    @spec_state.should be_kind_of(SpecState)
  end
end

describe SpecState do
  it "is initialized with the describe and it strings" do
    SpecState.new("This", "does").should be_kind_of(SpecState)
  end
end

describe SpecState, "#describe" do
  before :each do
    @state = SpecState.new("describe", "it")
  end

  it "returns the arguments to the #describe block stringified and concatenated" do
    @state.describe.should == "describe"
  end
end

describe SpecState, "#it" do
  before :each do
    @state = SpecState.new("describe", "it")
  end

  it "returns the argument to the #it block" do
    @state.it.should == "it"
  end
end

describe SpecState, "#exceptions" do
  before :each do
    @state = SpecState.new("describe", "it")
  end
  
  it "returns an array" do
    @state.exceptions.should be_kind_of(Array)
  end
end

describe SpecState, "#exception?" do
  before :each do
    @state = SpecState.new("describe", "it")
  end
  
  it "returns false if no exceptions were recorded" do
    @state.exception?.should == false
  end
  
  it "returns true if any exceptions were recorded" do
    @state.exceptions.push :a
    @state.exception?.should == true
  end
end
