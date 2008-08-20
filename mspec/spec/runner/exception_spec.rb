require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/runner/example'
require 'mspec/runner/exception'

describe ExceptionState, "#initialize" do
  it "takes a state, location (e.g. before :each), and exception" do
    context = ContextState.new "Class#method"
    state = ExampleState.new context, "does something"
    exc = Exception.new "Fail!"
    ExceptionState.new(state, "location", exc).should be_kind_of(ExceptionState)
  end
end

describe ExceptionState, "#description" do
  before :each do
    context = ContextState.new "Class#method"
    @state = ExampleState.new context, "does something"
  end

  it "returns the state description if state was not nil" do
    exc = ExceptionState.new(@state, nil, nil)
    exc.description.should == "Class#method does something"
  end

  it "returns the location if it is not nil and description is nil" do
    exc = ExceptionState.new(nil, "location", nil)
    exc.description.should == "An exception occurred during: location"
  end

  it "returns both description and location if neither are nil" do
    exc = ExceptionState.new(@state, "location", nil)
    exc.description.should == "An exception occurred during: location\nClass#method does something"
  end
end

describe ExceptionState, "#describe" do
  before :each do
    context = ContextState.new "Class#method"
    @state = ExampleState.new context, "does something"
  end

  it "returns the ExampleState#describe string if created with a non-nil state" do
    ExceptionState.new(@state, nil, nil).describe.should == @state.describe
  end

  it "returns an empty string if created with a nil state" do
    ExceptionState.new(nil, nil, nil).describe.should == ""
  end
end

describe ExceptionState, "#it" do
  before :each do
    context = ContextState.new "Class#method"
    @state = ExampleState.new context, "does something"
  end

  it "returns the ExampleState#it string if created with a non-nil state" do
    ExceptionState.new(@state, nil, nil).it.should == @state.it
  end

  it "returns an empty string if created with a nil state" do
    ExceptionState.new(nil, nil, nil).it.should == ""
  end
end

describe ExceptionState, "#failure?" do
  before :each do
    @state = ExampleState.new ContextState.new("C#m"), "works"
  end

  it "returns true if the exception is an ExpectationNotMetError" do
    exc = ExceptionState.new @state, "", ExpectationNotMetError.new("Fail!")
    exc.failure?.should be_true
  end

  it "returns true if the exception is an ExpectationNotFoundError" do
    exc = ExceptionState.new @state, "", ExpectationNotFoundError.new("Fail!")
    exc.failure?.should be_true
  end

  it "returns false if the exception is not an ExpectationNotMetError or an ExpectationNotFoundError" do
    exc = ExceptionState.new @state, "", Exception.new("Fail!")
    exc.failure?.should be_false
  end
end

describe ExceptionState, "#message" do
  it "returns <No message> if the exception message is empty" do
    exc = ExceptionState.new @state, "", Exception.new("")
    exc.message.should == "<No message>"
  end

  it "returns the message without exception class when the exception is an ExpectationNotMetError" do
    exc = ExceptionState.new @state, "", ExpectationNotMetError.new("Fail!")
    exc.message.should == "Fail!"
  end

  it "returns ExpectationNotFoundError#message when the exception is an ExpectationNotFoundError" do
    e = ExpectationNotFoundError.new
    exc = ExceptionState.new @state, "", e
    exc.message.should == e.message
  end

  it "returns the message with exception class when the exception is not an ExpectationNotMetError or an ExpectationNotFoundError" do
    exc = ExceptionState.new @state, "", Exception.new("Fail!")
    exc.message.should == "Exception: Fail!"
  end
end

describe ExceptionState, "#backtrace" do
  before :each do
    @action = mock("action")
    def @action.exception(exc)
      ScratchPad.record exc.exception
    end
    MSpec.register :exception, @action

    ScratchPad.clear
    MSpec.protect("") { raise Exception }

    @exc = ExceptionState.new @state, "", ScratchPad.recorded
  end

  it "returns a string representation of the exception backtrace" do
    @exc.backtrace.should be_kind_of(String)
  end

  it "strips MSpec files from the backtrace" do
    @exc.backtrace.split("\n").each do |line|
      line.should_not =~ ExceptionState::PATH
    end
  end
end
