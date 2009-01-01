require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#wakeup" do
  it "does not result in a deadlock" do
    c = Channel.new
    t1 = Thread.new do
      loop do
        c << t1
        c << t1
        Thread.stop
      end
    end
    t2 = Thread.new do
      loop do
        c << t2
        Thread.stop
      end
    end

    count1 = 0
    count2 = 0
    while(count1 < 10) do
      if c.size > 0
        case c.receive
        when t1; count1+=1
        when t2; count2+=1
        end
      end
      t1.wakeup
      t2.wakeup
    end
    count1.should > count2

    t1.kill
    t2.kill
  end
end
