require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

describe "Readline::HISTORY.each" do
  before(:each) do
    Readline::HISTORY.push("1", "2", "3")
  end
  
  after(:each) do
    Readline::HISTORY.pop
    Readline::HISTORY.pop
    Readline::HISTORY.pop
  end
  
  it "yields each item in the history" do
    result = []
    Readline::HISTORY.each do |x|
      result << x
    end
    result.should == ["1", "2", "3"]
  end

  it "yields tainted Objects" do
    Readline::HISTORY.each do |x|
      x.tainted?.should be_true
    end
  end
end
end
