require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#initialize_copy" do
  before(:each) do
    @s      = StringScanner.new("StringScanner example")
    @orig_s = StringScanner.new("Original StringScanner")
  end

  it "is a private method" do
    @s.private_methods.should include("initialize")
  end

  it "copies the passed StringScanner's content to self" do
    @s.send(:initialize_copy, @orig_s)
    @s.string.should == "Original StringScanner"
  end

  it "copies the passed StringSCanner's position to self" do
    @orig_s.pos = 5
    @s.send(:initialize_copy, @orig_s)
    @s.pos.should eql(5)
  end

  it "does not taint self when the passed StringScanner is tainted" do
    @orig_s.taint
    @s.send(:initialize_copy, @orig_s)
    @s.tainted?.should be_false
  end

  it "copies the passed StringScanner scan pointer to self" do
    @orig_s.terminate
    @s.send(:initialize_copy, @orig_s)
    @s.eos?.should be_true
  end
end
