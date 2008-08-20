require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#dup" do
  before :each do
    @s = StringScanner.new("This is a test")
    @d = @s.dup
  end

  it "returns a new StringScanner duplicated from the original" do    
    @d.should be_kind_of(StringScanner)
    @d.inspect.should == @s.inspect
    @d.string.should == @s.string
    @d.eos?.should == @s.eos?
    @d.matched?.should == @s.matched?
    @d.pos.should == @s.pos
  end
end 
