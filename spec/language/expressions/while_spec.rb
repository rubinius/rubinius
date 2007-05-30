require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: break, redo, next, retry

# while bool-expr [do]
#   body
# end
#
# begin
#   body
# end until bool-expr
#
# expr while bool-expr
describe "The while expression" do
  it "should run while the expression is true" do
    i = 0
    while i < 3
      i += 1
    end
    i.should == 3
  end
  
  it "should optionally take a 'do' after the expression" do
    i = 0
    while i < 3 do
      i += 1
    end
    
    i.should == 3
  end
  
  it "should allow body begin on the same line if do is used" do
    i = 0
    while i < 3 do i += 1
    end
    
    i.should == 3
  end
  
  it "should execute code in containing variable scope" do
    i = 0
    while i != 1
      a = 123
      i = 1
    end
    
    a.should == 123
  end
  
  it "should execute code in containing variable scope with 'do'" do
    i = 0
    while i != 1 do
      a = 123
      i = 1
    end
    
    a.should == 123
  end
  
  it "should return nil if ended when condition became false" do
    i = 0
    while i < 3
      i += 1
    end.should == nil
  end
  
  it "should stop running body if interrupted by break" do
    i = 0
    while i < 10
      i += 1
      break if i > 5
    end
    i.should == 6
  end
  
  it "should return value passed to break if interrupted by break" do
    while true
      break 123
    end.should == 123
  end
  
  it "should return nil if interrupted by break with no arguments" do
    while true
      break
    end.should == nil
  end

end
  
describe "The postfix while expression" do
  it "should run preceding statement while the condition is true" do
    i = 0
    i += 1 while i < 3
    i.should == 3
  end
  
  it "should not run preceding statement if the condition is false" do
    i = 0
    i += 1 while false
    i.should == 0
  end
  
  it "should return nil if ended when condition became false" do
    i = 0
    (i += 1 while i<10).should == nil
  end
  
  it "should return value passed to break if interrupted by break" do
    (break 123 while true).should == 123
  end
  
  it "should return nil if interrupted by break with no arguments" do
    (break while true).should == nil
  end

end


describe "The postfix while expression with preceding begin .. end" do
  
  it "should run block while the expression is true" do
    i = 0
    begin
      i += 1
    end while i < 3
    
    i.should == 3
  end
  
  it "should stop running block if interrupted by break" do
    i = 0
    begin
      i += 1
      break if i > 5
    end while i < 10
    
    i.should == 6
  end
  
  it "should return value passed to break if interrupted by break" do
    (begin; break 123; end while true).should == 123
  end
  
  it "should return nil if interrupted by break with no arguments" do
    (begin; break; end while true).should == nil
  end
  
  it "should run block at least once (even if the expression is false)" do
    i = 0
    begin
      i += 1
    end while false
    
    i.should == 1
  end
end
