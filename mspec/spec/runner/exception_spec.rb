require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/expectations/expectations'
require 'mspec/runner/example'
require 'mspec/runner/exception'

describe ExceptionState, "#initialize" do
  it "takes a state, location (e.g. before :each), and exception" do
    state = ExampleState.new "Class#method", "does something"
    exc = Exception.new "Fail!"
    ExceptionState.new(state, "location", exc).should be_kind_of(ExceptionState)
  end
end

describe ExceptionState, "#description" do
  before :each do
    @state = ExampleState.new "Class#method", "does something"
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
    @state = ExampleState.new "Class#method", "does something"
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
    @state = ExampleState.new "Class#method", "does something"
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
    @state = ExampleState.new "C#m", "works"
  end

  it "returns true if the exception is an ExpectationNotMetError" do
    exc = ExceptionState.new @state, "", ExpectationNotMetError.new("Fail!")
    exc.failure?.should be_true
  end

  it "returns false if the exception is not an ExpectationNotMetError" do
    exc = ExceptionState.new @state, "", Exception.new("Fail!")
    exc.failure?.should be_false
  end
end

describe ExceptionState, "#message" do
  it "returns <No message> if the exception message is empty" do
    exc = ExceptionState.new @state, "", Exception.new("")
    exc.message.should == "<No message>"
  end

  it "returns the message without exception class when the exception is ExpectationNotMetError" do
    exc = ExceptionState.new @state, "", ExpectationNotMetError.new("Fail!")
    exc.message.should == "Fail!"
  end

  it "returns the message with exception class when the exception is not ExpectationNotMetError" do
    exc = ExceptionState.new @state, "", Exception.new("Fail!")
    exc.message.should == "Exception: Fail!"
  end
end

describe ExceptionState, "#backtrace" do
  before :each do
    begin
      raise Exception, "mock backtrace"
    rescue Exception => @exception
    end
  end

  it "returns a string representation of the exception backtrace" do
    exc = ExceptionState.new @state, "", @exception
    exc.backtrace.should be_kind_of(String)
  end

  # TODO: spec the filtering of the backtrace so mspec files don't display
end
