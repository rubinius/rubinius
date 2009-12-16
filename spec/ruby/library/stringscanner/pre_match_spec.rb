require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#pre_match" do
  before :each do
    @s = StringScanner.new("This is a test")
  end

  it "returns the pre-match (in the regular expression sense) of the last scan" do
    @s.pre_match.should == nil
    @s.scan(/\w+\s/)
    @s.pre_match.should == ""
    @s.getch
    @s.pre_match.should == "This "
    @s.get_byte
    @s.pre_match.should == "This i"
    @s.get_byte
    @s.pre_match.should == "This is"
  end

  it "returns nil if there's no match" do
    @s.scan(/\s+/)
    @s.pre_match.should == nil
  end
end
