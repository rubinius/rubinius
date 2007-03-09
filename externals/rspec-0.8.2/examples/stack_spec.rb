require File.dirname(__FILE__) + '/spec_helper'
require File.dirname(__FILE__) + "/stack"

context "A Stack" do
  setup do
    @stack = Stack.new
    ["a","b","c"].each { |x| @stack.push x }
  end
  
  specify "should add to the top when sent #push" do
    @stack.push "d"
    @stack.peek.should == "d"
  end
  
  specify "should return the top item when sent #peek" do
    @stack.peek.should == "c"
  end
  
  specify "should NOT remove the top item when sent #peek" do
    @stack.peek.should == "c"
    @stack.peek.should == "c"
  end
  
  specify "should return the top item when sent #pop" do
    @stack.pop.should == "c"
  end
  
  specify "should remove the top item when sent #pop" do
    @stack.pop.should == "c"
    @stack.pop.should == "b"
  end
end

context "An empty stack" do
  setup do
    @stack = Stack.new
  end
  
  # NOTE that this one auto-generates the description "should be_empty"
  specify { @stack.should be_empty }
  
  specify "should no longer be empty after #push" do
    @stack.push "anything"
    @stack.should_not be_empty
  end
  
  specify "should complain when sent #peek" do
    lambda { @stack.peek }.should raise_error(StackUnderflowError)
  end
  
  specify "should complain when sent #pop" do
    lambda { @stack.pop }.should raise_error(StackUnderflowError)
  end
end

context "An almost empty stack (with one item)" do
  setup do
    @stack = Stack.new
    @stack.push 3
  end
  
  # NOTE that this one auto-generates the description "should not be empty"
  specify { @stack.should_not be_empty }
  
  specify "should remain not empty after #peek" do
    @stack.peek
    @stack.should_not be_empty
  end
  
  specify "should become empty after #pop" do
    @stack.pop
    @stack.should be_empty
  end
end

context "An almost full stack (with one item less than capacity)" do
  setup do
    @stack = Stack.new
    (1..9).each { |i| @stack.push i }
  end
  
  # NOTE that this one auto-generates the description "should not be full"
  specify { @stack.should_not be_full }
  
  specify "should become full when sent #push" do
    @stack.push Object.new
    @stack.should be_full
  end
end

context "A full stack" do
  setup do
    @stack = Stack.new
    (1..10).each { |i| @stack.push i }
  end
  
  # NOTE that this one auto-generates the description "should be full"
  specify { @stack.should be_full }
  
  specify "should remain full after #peek" do
    @stack.peek
    @stack.should be_full
  end
  
  specify "should no longer be full after #pop" do
    @stack.pop
    @stack.should_not be_full
  end

  specify "should complain on #push" do
    lambda { @stack.push Object.new }.should raise_error(StackOverflowError)
  end
end