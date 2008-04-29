require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/matchers/base'
require 'mspec/runner/mspec'
require 'mspec/mocks/mock'
require 'mspec/runner/state'

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
    msg, proc = @state.instance_variable_get(:@spec)[0]
    msg.should == "message"
    proc.should == @proc
  end

  it "records describe blocks" do
    @state.describe(Object, "message", &@proc)
    @state.instance_variable_get(:@describe).should == "Object message"
    @state.instance_variable_get(:@block).should == @proc
  end
end

describe RunState, "#protect" do
  it "calls MSpec.protect" do
    ScratchPad.record []
    a = lambda { ScratchPad << :a }
    b = lambda { ScratchPad << :b }
    RunState.new.protect("message", [a, b])
    ScratchPad.recorded.should == [:a, :b]
  end
end

describe RunState, "#state" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = RunState.new
  end

  it "returns nil if no spec is being executed" do
    @state.state.should == nil
  end

  it "returns a SpecState instance if a spec is being executed" do
    ScratchPad.record @state
    @state.describe("") { }
    @state.it("") { ScratchPad.record ScratchPad.recorded.state }
    @state.process
    @state.state.should == nil
    ScratchPad.recorded.should be_kind_of(SpecState)
  end
end

describe RunState, "#process" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = RunState.new
    @state.describe("") { }

    @a = lambda { ScratchPad << :a }
    @b = lambda { ScratchPad << :b }
    ScratchPad.record []
  end

  it "calls each before(:all) block" do
    @state.before(:all, &@a)
    @state.before(:all, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == [:a, :b]
  end

  it "calls each after(:all) block" do
    @state.after(:all, &@a)
    @state.after(:all, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == [:a, :b]
  end

  it "calls each it block" do
    @state.it("one", &@a)
    @state.it("two", &@b)
    @state.process
    ScratchPad.recorded.should == [:a, :b]
  end

  it "calls each before(:each) block" do
    @state.before(:each, &@a)
    @state.before(:each, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == [:a, :b]
  end

  it "calls each after(:each) block" do
    @state.after(:each, &@a)
    @state.after(:each, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == [:a, :b]
  end

  it "calls Mock.cleanup for each it block" do
    @state.it("") { }
    @state.it("") { }
    Mock.should_receive(:cleanup).twice
    @state.process
  end

  it "calls Mock.verify_count for each it block" do
    @state.it("") { }
    @state.it("") { }
    Mock.should_receive(:verify_count).twice
    @state.process
  end

  it "calls the describe block" do
    ScratchPad.record []
    @state.describe(Object, "msg") { ScratchPad << :a }
    @state.process
    ScratchPad.recorded.should == [:a]
  end

  it "creates a new SpecState instance for each spec" do
    ScratchPad.record @state
    @state.describe("desc") { }
    @state.it("it") { ScratchPad.record ScratchPad.recorded.state }
    @state.process
    ScratchPad.recorded.should be_kind_of(SpecState)
  end

  it "records exceptions that occur while running the spec" do
    ScratchPad.record @state
    exception = Exception.new("bump!")
    MSpec.stack.push @state
    @state.describe("describe") { }
    @state.it("it") { raise exception }
    @state.after(:each) { ScratchPad.record ScratchPad.recorded.state.exceptions }
    @state.process
    ScratchPad.recorded.should == [[nil, exception]]
  end

  it "shuffles the spec list if MSpec.randomize? is true" do
    MSpec.randomize
    MSpec.should_receive(:shuffle)
    @state.it("") { }
    @state.process
    MSpec.randomize false
  end
end

describe RunState, "#process in pretend mode" do
  before :all do
    MSpec.register_mode :pretend
  end

  after :all do
    MSpec.register_mode nil
  end

  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = RunState.new
    @state.describe("") { }

    @a = lambda { ScratchPad << :a }
    @b = lambda { ScratchPad << :b }
    ScratchPad.record []
  end

  it "does not call any before(:all) block" do
    @state.before(:all, &@a)
    @state.before(:all, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call any after(:all) block" do
    @state.after(:all, &@a)
    @state.after(:all, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call any it block" do
    @state.it("one", &@a)
    @state.it("two", &@b)
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call any before(:each) block" do
    @state.before(:each, &@a)
    @state.before(:each, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call any after(:each) block" do
    @state.after(:each, &@a)
    @state.after(:each, &@b)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call Mock.cleanup" do
    @state.it("") { }
    @state.it("") { }
    Mock.should_not_receive(:cleanup)
    @state.process
  end

  it "calls the describe block" do
    ScratchPad.record []
    @state.describe(Object, "msg") { ScratchPad << :a }
    @state.process
    ScratchPad.recorded.should == [:a]
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

  after :each do
    MSpec.store :before, nil
    MSpec.store :after, nil
  end

  it "calls registered before actions with the current SpecState instance" do
    before = mock("before")
    before.should_receive(:before).and_return {
      ScratchPad.record :before
      @spec_state = @state.state
    }
    MSpec.register :before, before
    @state.process
    ScratchPad.recorded.should == :before
    @spec_state.should be_kind_of(SpecState)
  end

  it "calls registered after actions with the current SpecState instance" do
    after = mock("after")
    after.should_receive(:after).and_return {
      ScratchPad.record :after
      @spec_state = @state.state
    }
    MSpec.register :after, after
    @state.process
    ScratchPad.recorded.should == :after
    @spec_state.should be_kind_of(SpecState)
  end
end

describe RunState, "#process in pretend mode" do
  before :all do
    MSpec.register_mode :pretend
  end

  after :all do
    MSpec.register_mode nil
  end

  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = RunState.new
    @state.describe("") { }
    @state.it("") { }
  end

  after :each do
    MSpec.store :before, nil
    MSpec.store :after, nil
  end

  it "calls registered before actions with the current SpecState instance" do
    before = mock("before")
    before.should_receive(:before).and_return {
      ScratchPad.record :before
      @spec_state = @state.state
    }
    MSpec.register :before, before
    @state.process
    ScratchPad.recorded.should == :before
    @spec_state.should be_kind_of(SpecState)
  end

  it "calls registered after actions with the current SpecState instance" do
    after = mock("after")
    after.should_receive(:after).and_return {
      ScratchPad.record :after
      @spec_state = @state.state
    }
    MSpec.register :after, after
    @state.process
    ScratchPad.recorded.should == :after
    @spec_state.should be_kind_of(SpecState)
  end
end

describe RunState, "#process" do
  before :each do
    MSpec.store :enter, []
    MSpec.store :leave, []

    @state = RunState.new
    @state.describe("") { }
    @state.it("") { }
  end

  after :each do
    MSpec.store :enter, nil
    MSpec.store :leave, nil
  end

  it "calls registered enter actions with the current #describe string" do
    enter = mock("enter")
    enter.should_receive(:enter).and_return { ScratchPad.record :enter }
    MSpec.register :enter, enter
    @state.process
    ScratchPad.recorded.should == :enter
  end

  it "calls registered leave actions" do
    leave = mock("leave")
    leave.should_receive(:leave).and_return { ScratchPad.record :leave }
    MSpec.register :leave, leave
    @state.process
    ScratchPad.recorded.should == :leave
  end
end

describe RunState, "#process in pretend mode" do
  before :all do
    MSpec.register_mode :pretend
  end

  after :all do
    MSpec.register_mode nil
  end

  before :each do
    MSpec.store :enter, []
    MSpec.store :leave, []

    @state = RunState.new
    @state.describe("") { }
    @state.it("") { }
  end

  after :each do
    MSpec.store :enter, nil
    MSpec.store :leave, nil
  end

  it "calls registered enter actions with the current #describe string" do
    enter = mock("enter")
    enter.should_receive(:enter).and_return { ScratchPad.record :enter }
    MSpec.register :enter, enter
    @state.process
    ScratchPad.recorded.should == :enter
  end

  it "calls registered leave actions" do
    leave = mock("leave")
    leave.should_receive(:leave).and_return { ScratchPad.record :leave }
    MSpec.register :leave, leave
    @state.process
    ScratchPad.recorded.should == :leave
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

describe SpecState, "#unfiltered?" do
  before :each do
    MSpec.store :include, nil
    MSpec.store :exclude, nil

    @state = SpecState.new("describe", "it")
    @filter = mock("filter")
  end

  it "returns true if MSpec include filters list is empty" do
    @state.unfiltered?.should == true
  end

  it "returns true if MSpec include filters match this spec" do
    @filter.should_receive(:===).and_return(true)
    MSpec.register :include, @filter
    @state.unfiltered?.should == true
  end

  it "returns false if MSpec include filters do not match this spec" do
    @filter.should_receive(:===).and_return(false)
    MSpec.register :include, @filter
    @state.unfiltered?.should == false
  end

  it "returns true if MSpec exclude filters list is empty" do
    @state.unfiltered?.should == true
  end

  it "returns true if MSpec exclude filters do not match this spec" do
    @filter.should_receive(:===).and_return(false)
    MSpec.register :exclude, @filter
    @state.unfiltered?.should == true
  end

  it "returns false if MSpec exclude filters match this spec" do
    @filter.should_receive(:===).and_return(true)
    MSpec.register :exclude, @filter
    @state.unfiltered?.should == false
  end

  it "returns false if MSpec include and exclude filters match this spec" do
    @filter.should_receive(:===).twice.and_return(true)
    MSpec.register :include, @filter
    MSpec.register :exclude, @filter
    @state.unfiltered?.should == false
  end
end

describe SpecState, "#filtered?" do
  before :each do
    @state = SpecState.new("describe", "it")
  end

  it "returns true if #unfiltered returns false" do
    @state.should_receive(:unfiltered?).and_return(false)
    @state.filtered?.should == true
  end

  it "returns false if #unfiltered returns true" do
    @state.should_receive(:unfiltered?).and_return(true)
    @state.filtered?.should == false
  end
end

describe SpecState, "#failure?" do
  before :each do
    @state = SpecState.new("describe", "it")
  end
end
