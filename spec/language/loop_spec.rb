require File.dirname(__FILE__) + '/../spec_helper'

describe "The loop expression" do
  
  it "repeats the given block until a break is called" do
    i = 0
    loop do
      i += 1
      break if i == 10
    end
    
    i.should == 10
  end
  
  it "executes code in its own scope" do
    loop do
      a = 123
      break
    end
    
    should_raise(NameError) { a }
  end
  
  it "returns the value passed to break if interrupted by break" do
    loop do
      break 123
    end.should == 123
  end
  
  it "returns nil if interrupted by break with no arguments" do
    loop do
      break
    end.should == nil
  end

  it "skips to end of body with next" do
    a = []
    i = 0
    loop do
      break if (i+=1) >= 5
      next if i == 3
      a << i
    end
    a.should == [1, 2, 4]
  end
  
  it "restarts the current iteration with redo" do
    a = []
    loop do
      a << 1
      redo if a.size < 2
      a << 2
      break if a.size == 3      
    end
    a.should == [1, 1, 2]
  end
  
end