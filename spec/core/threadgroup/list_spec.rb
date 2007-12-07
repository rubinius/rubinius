require File.dirname(__FILE__) + '/../../spec_helper'

describe "ThreadGroup#list" do
  it "should return the list of threads in the group" do
    thread = Thread.new { }
    tg = ThreadGroup.new
    tg.add(thread)
    tg.list.should == [thread]
    
    thread2 = Thread.new { }
    tg.add(thread2)
    
    (tg.list & [thread, thread2]).should == [thread, thread2]

    thread.join
    thread2.join
  end
end