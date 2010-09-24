require File.expand_path('../../../spec_helper', __FILE__)
require 'strscan'

describe "StringScanner#initialize_copy" do
  before(:each) do
    @s      = StringScanner.new("StringScanner example")
    @string = "this is a test"
    @orig_s = StringScanner.new(@string)
  end

  it "is a private method" do
    StringScanner.should have_private_instance_method(:initialize_copy)
  end

  it "copies the passed StringScanner's content to self" do
    @s.send(:initialize_copy, @orig_s)
    @s.string.should == @string
  end

  it "copies the passed StringSCanner's position to self" do
    @orig_s.pos = 5
    @s.send(:initialize_copy, @orig_s)
    @s.pos.should eql(5)
  end

  it "copies previous match state" do
    @orig_s.scan(/\w+/)
    @orig_s.scan(/\s/)

    @orig_s.pre_match.should == "this"

    @s.send(:initialize_copy, @orig_s)
    @s.pre_match.should == "this"

    @s.unscan
    @s.scan(/\s/).should == " "
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
