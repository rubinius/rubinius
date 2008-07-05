require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/matchers/base'
require 'mspec/runner/mspec'
require 'mspec/mocks/mock'
require 'mspec/runner/example'

describe ExampleState do
  it "is initialized with the describe and it strings" do
    ExampleState.new("This", "does").should be_kind_of(ExampleState)
  end
end

describe ExampleState, "#describe" do
  before :each do
    @state = ExampleState.new("describe", "it")
  end

  it "returns the arguments to the #describe block stringified and concatenated" do
    @state.describe.should == "describe"
  end
end

describe ExampleState, "#it" do
  before :each do
    @state = ExampleState.new("describe", "it")
  end

  it "returns the argument to the #it block" do
    @state.it.should == "it"
  end
end

describe ExampleState, "#unfiltered?" do
  before :each do
    MSpec.store :include, nil
    MSpec.store :exclude, nil

    @state = ExampleState.new("describe", "it")
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

describe ExampleState, "#filtered?" do
  before :each do
    @state = ExampleState.new("describe", "it")
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
