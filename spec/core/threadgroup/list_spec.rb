require File.dirname(__FILE__) + '/../../spec_helper'

describe "ThreadGroup#list" do
  it "should return the list of threads in the group" do
    thread = Thread.new { sleep }
    Thread.pass until thread.status == 'sleep'
    tg = ThreadGroup.new
    tg.add(thread)
    tg.list.should include(thread)
    
    thread2 = Thread.new { sleep }
    Thread.pass until thread2.status == 'sleep'
    tg.add(thread2)
    
    (tg.list & [thread, thread2]).should include(thread, thread2)

    thread.run; thread.join
    thread2.run; thread2.join
  end
end
