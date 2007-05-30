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
      a = 0
      i += 1
    end
    
    a.should == 0
    i.should == 3
  end
  
  it "should have no return value" do
    i = 0
    val = while i < 3
      i += 1
    end.should == nil
    val.should == nil
  end
  
  it "should run it's preceding statement while the expression is true" do
    i = 0
    i += 1 while i < 3
    i.should == 3
  end
  
  it "should run a preceding begin .. end block while the expression is true" do
    i = 0
    begin
      i += 1
    end while i < 3
    
    i.should == 3
  end
  
  it "should run a preceding begin .. end block at least once (even if the expression is false)" do
    i = 0
    begin
      i += 1
    end while false
    
    i.should == 1
  end
end