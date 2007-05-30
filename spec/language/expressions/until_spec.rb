require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: break, redo, next, retry

# until bool-expr [do]
#   body
# end
# 
# begin
#   body
# end while bool-expr
# 
# expr until bool-expr
describe "The until expression" do
  it "should run while the expression is false" do
    i = 0
    until i > 9
      i += 1
    end
    
    i.should == 10
  end
  
  it "should optionally take a do after the expression" do
    i = 0
    until i > 9 do a = 0; i += 1; end
    
    a.should == 0
    i.should == 10
  end

  it "should have no return value" do
    i = 0
    val = until i > 9
      i += 1
    end.should == nil
    val.should == nil
  end

  it "should run it's preceding statement while the expression is false" do
    i = 0
    i += 1 until i > 9
    i.should == 10
  end

  it "should run a preceding begin .. end block while the expression is false" do
    i = 0
    begin
      i += 1
    end until i > 9
    
    i.should == 10
  end
  
  it "should run a preceding begin .. end block at least once (even if the expression is true)" do
    i = 0
    begin
      i += 1
    end until true
    
    i.should == 1
  end
end
