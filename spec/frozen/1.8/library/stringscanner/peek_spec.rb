require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#peek" do
  it "returns at most the specified number of characters from the current position" do
    s = StringScanner.new "peek spec"
    s.pos = 5
    s.peek(2).should == "sp"
    s.peek(1000).should == "spec"
  end
end
