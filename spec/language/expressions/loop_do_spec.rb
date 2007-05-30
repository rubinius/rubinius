require File.dirname(__FILE__) + '/../../spec_helper'

# * break terminates loop immediately.
# * redo immediately repeats w/o rerunning the condition.
# * next starts the next iteration through the loop.
# * retry restarts the loop, rerunning the condition.

# TODO: break, redo, next, retry

# loop do
#   body
# end
describe "The loop" do
  it "should run until it is terminated by a break" do
    i = 0
    loop do
      i += 1
      break if i == 10
    end.should == nil
    
    i.should == 10
  end
end