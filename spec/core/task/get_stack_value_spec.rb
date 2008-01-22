require File.dirname(__FILE__) + '/../../spec_helper'

describe "Task#get_stack_value" do
  it "throws an ArgumentError if depth is out of range" do
    lambda { Task.current.get_stack_value(0) }.should_not raise_error(ArgumentError)
    lambda { Task.current.get_stack_value(-1) }.should raise_error(ArgumentError)
    lambda { Task.current.get_stack_value(Task.current.stack_size) }.should raise_error(ArgumentError)
    lambda { Task.current.get_stack_value(Task.current.stack_size-1) }.should_not raise_error(ArgumentError)
  end
end