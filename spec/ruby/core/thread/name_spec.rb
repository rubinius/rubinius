# encoding: utf-8
require File.expand_path('../../../spec_helper', __FILE__)

describe "Thread#name=" do
  before :each do
    @thread = Thread.new { }
  end

  after :each do
    @thread.join
  end

  it "sets the Thread name" do
    @thread.name = "a new name"
    @thread.name.should == "a new name"
  end

  it "ignores setting the Thread's name to nil" do
    @thread.name = nil
    @thread.name.should be_an_instance_of(String)
  end

  it "calls #to_str to convert name to String" do
    name = mock("Thread#name")
    name.should_receive(:to_str).and_return("a thread name")

    @thread.name = name
    @thread.name.should == "a thread name"
  end

  it "raises an ArgumentError if the name is not ASCII" do
    lambda { @thread.name = "あ" }.should raise_error(ArgumentError)
  end
end

describe "Thread#name" do
  it "returns a String" do
    @thread.name.should be_an_instance_of(String)
  end
end
