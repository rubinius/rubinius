require File.dirname(__FILE__) + '/../../../spec_helper'

has_tty? do # needed for CI until we figure out a better way
require 'readline'

describe "Readline::HISTORY.[]" do
  before(:each) do
    Readline::HISTORY.push("1", "2", "3")
  end
  
  after(:each) do
    Readline::HISTORY.pop
    Readline::HISTORY.pop
    Readline::HISTORY.pop
  end
  
  it "returns tainted objects" do
    Readline::HISTORY[0].tainted?.should be_true
    Readline::HISTORY[1].tainted?.should be_true
  end

  # This behaviour is a potential bug caused by
  # Mac OS X use of editline instead of Readline.
  # This bug should also exist on BSDs.
  platform_is :darwin do
    it "returns the history item at the passed index + 1" do
      Readline::HISTORY[0].should == "2"
      Readline::HISTORY[1].should == "3"
    
      # NOTE: This is weird behaviour!
      # One would expect -1 to return "3"
      Readline::HISTORY[-2].should == "3"
      Readline::HISTORY[-3].should == "2"
      Readline::HISTORY[-4].should == "1"
    end

    it "returns the first item when there is no item at the passed negative index" do
      # NOTE: This is weird behaviour!
      # One would expect these to raise an IndexError
      Readline::HISTORY[-10].should == "1"
      Readline::HISTORY[-9].should == "1"
      Readline::HISTORY[-8].should == "1"
    end

    it "raises an IndexError when there is no item at the passed positive index" do
      lambda { Readline::HISTORY[10] }.should raise_error(IndexError)
    end

    it "raises an IndexError when passed -1 as index" do
      lambda { Readline::HISTORY[-1] }.should raise_error(IndexError)
    end
  end
  
  platform_is_not :darwin do
    it "returns the history item at the passed index" do
      Readline::HISTORY[0].should == "1"
      Readline::HISTORY[1].should == "2"
      Readline::HISTORY[2].should == "3"

      Readline::HISTORY[-1].should == "3"
      Readline::HISTORY[-2].should == "2"
      Readline::HISTORY[-3].should == "1"
    end

    it "raises an IndexError when there is no item at the passed index" do
      lambda { Readline::HISTORY[-10] }.should raise_error(IndexError)
      lambda { Readline::HISTORY[-9] }.should raise_error(IndexError)
      lambda { Readline::HISTORY[-8] }.should raise_error(IndexError)

      lambda { Readline::HISTORY[8] }.should raise_error(IndexError)
      lambda { Readline::HISTORY[9] }.should raise_error(IndexError)
      lambda { Readline::HISTORY[10] }.should raise_error(IndexError)
    end
  end
end
end
