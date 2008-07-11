require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/matchers/base'
require 'mspec/runner/mspec'
require 'mspec/mocks/mock'
require 'mspec/runner/context'

describe ContextState do
  before :each do
    @state = ContextState.new
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

describe ContextState, "#protect" do
  before :each do
    ScratchPad.record []
    @a = lambda { ScratchPad << :a }
    @b = lambda { ScratchPad << :b }
    @c = lambda { raise Exception, "Fail!" }
  end

  it "returns true and does execute any blocks if check is true and MSpec.pretend_mode? is true" do
    MSpec.stub!(:pretend_mode?).and_return(true)
    ContextState.new.protect("message", [@a, @b]).should be_true
    ScratchPad.recorded.should == []
  end

  it "executes the blocks if MSpec.pretend_mode? is false" do
    MSpec.stub!(:pretend_mode?).and_return(false)
    ContextState.new.protect("message", [@a, @b])
    ScratchPad.recorded.should == [:a, :b]
  end

  it "executes the blocks if check is false" do
    ContextState.new.protect("message", [@a, @b], false)
    ScratchPad.recorded.should == [:a, :b]
  end

  it "returns true if none of the blocks raise an exception" do
    ContextState.new.protect("message", [@a, @b]).should be_true
  end

  it "returns false if any of the blocks raise an exception" do
    ContextState.new.protect("message", [@a, @c, @b]).should be_false
  end
end

describe ContextState, "#state" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
  end

  it "returns nil if no spec is being executed" do
    @state.state.should == nil
  end

  it "returns a ExampleState instance if an example is being executed" do
    ScratchPad.record @state
    @state.describe("") { }
    @state.it("") { ScratchPad.record ScratchPad.recorded.state }
    @state.process
    @state.state.should == nil
    ScratchPad.recorded.should be_kind_of(ExampleState)
  end
end

describe ContextState, "#process" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
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

  it "creates a new ExampleState instance for each example" do
    ScratchPad.record @state
    @state.describe("desc") { }
    @state.it("it") { ScratchPad.record ScratchPad.recorded.state }
    @state.process
    ScratchPad.recorded.should be_kind_of(ExampleState)
  end

  it "clears the expectations flag before evaluating the #it block" do
    MSpec.clear_expectations
    MSpec.should_receive(:clear_expectations)
    @state.it("it") { ScratchPad.record MSpec.expectation? }
    @state.process
    ScratchPad.recorded.should be_false
  end

  it "shuffles the spec list if MSpec.randomize? is true" do
    MSpec.randomize
    MSpec.should_receive(:shuffle)
    @state.it("") { }
    @state.process
    MSpec.randomize false
  end
end

describe ContextState, "#process" do
  before :each do
    MSpec.store :exception, []

    @state = ContextState.new
    @state.describe("") { }

    action = mock("action")
    def action.exception(exc)
      ScratchPad.record :exception if exc.exception.is_a? ExpectationNotFoundError
    end
    MSpec.register :exception, action

    MSpec.clear_expectations
    ScratchPad.clear
  end

  after :each do
    MSpec.store :exception, nil
  end

  it "raises an ExpectationNotFoundError if an #it block does not contain an expectation" do
    @state.it("it") { }
    @state.process
    ScratchPad.recorded.should == :exception
  end

  it "does not raise an ExpectationNotFoundError if an #it block does contain an expectation" do
    @state.it("it") { MSpec.expectation }
    @state.process
    ScratchPad.recorded.should be_nil
  end

  it "does not raise an ExpectationNotFoundError if the #it block causes a failure" do
    @state.it("it") { raise Exception, "Failed!" }
    @state.process
    ScratchPad.recorded.should be_nil
  end
end

describe ContextState, "#process" do
  before :each do
    MSpec.store :example, []

    @state = ContextState.new
    @state.describe("") { }

    example = mock("example")
    def example.example(state, spec)
      ScratchPad << state << spec
    end
    MSpec.register :example, example

    ScratchPad.record []
  end

  after :each do
    MSpec.store :example, nil
  end

  it "calls registered example actions with the current ExampleState and block" do
    @state.it("") { MSpec.expectation }
    @state.process

    ScratchPad.recorded.first.should be_kind_of(ExampleState)
    ScratchPad.recorded.last.should be_kind_of(Proc)
  end

  it "does not call registered example actions if the example has no block" do
    @state.it("empty example")
    @state.process
    ScratchPad.recorded.should == []
  end
end

describe ContextState, "#process" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
    @state.describe("") { }
    @state.it("") { MSpec.expectation }
  end

  after :each do
    MSpec.store :before, nil
    MSpec.store :after, nil
  end

  it "calls registered before actions with the current ExampleState instance" do
    before = mock("before")
    before.should_receive(:before).and_return {
      ScratchPad.record :before
      @spec_state = @state.state
    }
    MSpec.register :before, before
    @state.process
    ScratchPad.recorded.should == :before
    @spec_state.should be_kind_of(ExampleState)
  end

  it "calls registered after actions with the current ExampleState instance" do
    after = mock("after")
    after.should_receive(:after).and_return {
      ScratchPad.record :after
      @spec_state = @state.state
    }
    MSpec.register :after, after
    @state.process
    ScratchPad.recorded.should == :after
    @spec_state.should be_kind_of(ExampleState)
  end
end

describe ContextState, "#process" do
end

describe ContextState, "#process" do
  before :each do
    MSpec.store :enter, []
    MSpec.store :leave, []

    @state = ContextState.new
    @state.describe("") { }
    @state.it("") { MSpec.expectation }
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

describe ContextState, "#process when an exception is raised in before(:all)" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
    @state.describe("") { }

    @a = lambda { ScratchPad << :a }
    @b = lambda { ScratchPad << :b }
    ScratchPad.record []

    @state.before(:all) { raise Exception, "Fail!" }
  end

  after :each do
    MSpec.store :before, nil
    MSpec.store :after, nil
  end

  it "does not call before(:each)" do
    @state.before(:each, &@a)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call the it block" do
    @state.it("one", &@a)
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call after(:each)" do
    @state.after(:each, &@a)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call after(:each)" do
    @state.after(:all, &@a)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call Mock.verify_count" do
    @state.it("") { }
    Mock.should_not_receive(:verify_count)
    @state.process
  end

  it "calls Mock.cleanup" do
    @state.it("") { }
    Mock.should_receive(:cleanup)
    @state.process
  end
end

describe ContextState, "#process when an exception is raised in before(:each)" do
  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
    @state.describe("") { }

    @a = lambda { ScratchPad << :a }
    @b = lambda { ScratchPad << :b }
    ScratchPad.record []

    @state.before(:each) { raise Exception, "Fail!" }
  end

  after :each do
    MSpec.store :before, nil
    MSpec.store :after, nil
  end

  it "does not call the it block" do
    @state.it("one", &@a)
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call after(:each)" do
    @state.after(:each, &@a)
    @state.it("") { }
    @state.process
    ScratchPad.recorded.should == []
  end

  it "does not call Mock.verify_count" do
    @state.it("") { }
    Mock.should_not_receive(:verify_count)
    @state.process
  end
end

describe ContextState, "#process in pretend mode" do
  before :all do
    MSpec.register_mode :pretend
  end

  after :all do
    MSpec.register_mode nil
  end

  before :each do
    ScratchPad.clear
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
    @state.describe("") { }
    @state.it("") { }
  end

  after :each do
    MSpec.store :before, nil
    MSpec.store :after, nil
  end

  it "calls registered before actions with the current ExampleState instance" do
    before = mock("before")
    before.should_receive(:before).and_return {
      ScratchPad.record :before
      @spec_state = @state.state
    }
    MSpec.register :before, before
    @state.process
    ScratchPad.recorded.should == :before
    @spec_state.should be_kind_of(ExampleState)
  end

  it "calls registered after actions with the current ExampleState instance" do
    after = mock("after")
    after.should_receive(:after).and_return {
      ScratchPad.record :after
      @spec_state = @state.state
    }
    MSpec.register :after, after
    @state.process
    ScratchPad.recorded.should == :after
    @spec_state.should be_kind_of(ExampleState)
  end
end

describe ContextState, "#process in pretend mode" do
  before :all do
    MSpec.register_mode :pretend
  end

  after :all do
    MSpec.register_mode nil
  end

  before :each do
    MSpec.store :before, []
    MSpec.store :after, []

    @state = ContextState.new
    @state.describe("") { }

    @a = lambda { ScratchPad << :a }
    @b = lambda { ScratchPad << :b }
    ScratchPad.record []
  end

  it "calls the describe block" do
    ScratchPad.record []
    @state.describe(Object, "msg") { ScratchPad << :a }
    @state.process
    ScratchPad.recorded.should == [:a]
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
end

describe ContextState, "#process in pretend mode" do
  before :all do
    MSpec.register_mode :pretend
  end

  after :all do
    MSpec.register_mode nil
  end

  before :each do
    MSpec.store :enter, []
    MSpec.store :leave, []

    @state = ContextState.new
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
