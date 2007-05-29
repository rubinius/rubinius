require File.dirname(__FILE__) + '/../spec_helper'

describe "The if expression" do
  it "evaluates body when the if expression is true" do
    if true: true; end.should == true
    if true then true; end.should == true
    if true
      true
    end.should == true
  end
  
  it "does not evaluate body when the if expression is false" do
    if false: true; end.should == nil
    if false then true; end.should == nil
    if false
      true
    end.should == nil
  end
  
  it "does not evaluate the else body when the if expression is true" do
    if true: true; else false; end.should == true
    if true then true; else false; end.should == true
    if true: true
    else
      false
    end.should == true
    if true
      true
    else
      false
    end.should == true
  end
  
  it "evaluates the else body when the if expression is false" do
    if false: true; else false; end.should == false
    if false then true; else false; end.should == false
    if false: true
    else
      false
    end.should == false
    if false
      true
    else
      false
    end.should == false
  end
end

describe "The unless expression" do
  it "evaluates the else body when the unless expression is false" do
    unless false: true; else false; end.should == true
    unless false then true; else false; end.should == true
    unless false: true
    else 
      false
    end.should == true
    unless false
      true
    else
      false
    end.should == true
  end
end

# * break terminates loop immediately.
# * redo immediately repeats w/o rerunning the condition.
# * next starts the next iteration through the loop.
# * retry restarts the loop, rerunning the condition.

describe "The loop" do
  # loop do
  #   body
  # end
end

describe "The while expression" do
  
  # while bool-expr [do]
  #  body
  # end

  # begin
  #  body
  # end until bool-expr

  # expr while bool-expr

end

describe "The until expression" do
  # until bool-expr [do]
  #  body
  # end

  # begin
  #  body
  # end while bool-expr

  # expr until bool-expr

end

describe "The for expression" do
  # for name[, name]... in expr [do]
  #   body
  # end
  it "should iterate over an Enumerable passing each element to the block" do
    j = 0
    for i in 1..3
      j += i
    end
    j.should == 6
  end

  it "should iterate over an Hash passing each key-value pair to the block" do
    hash = { 1 => 10, 2 => 20 }
    
    k = 0
    l = 0
    
    for i, j in hash
      k += i
      l += j
    end
    
    k.should == 3
    l.should == 30
  end
end
