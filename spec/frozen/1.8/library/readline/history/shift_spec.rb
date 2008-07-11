require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

describe "Readline::HISTORY.shift" do
  it "returns nil when the history is empty" do
    Readline::HISTORY.shift.should be_nil
  end
  
  it "returns and removes the first item from the history" do
    Readline::HISTORY.push("1", "2", "3")
    Readline::HISTORY.size.should == 3
    
    Readline::HISTORY.shift.should == "1"
    Readline::HISTORY.size.should == 2
    
    Readline::HISTORY.shift.should == "2"
    Readline::HISTORY.size.should == 1
    
    Readline::HISTORY.shift.should == "3"
    Readline::HISTORY.size.should == 0
  end

  it "taints the returned strings" do
    Readline::HISTORY.push("1", "2", "3")
    Readline::HISTORY.shift.tainted?.should be_true
    Readline::HISTORY.shift.tainted?.should be_true
    Readline::HISTORY.shift.tainted?.should be_true
  end
end
end
