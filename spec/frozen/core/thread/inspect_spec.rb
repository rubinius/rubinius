require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#inspect" do
  it "can check it's own status" do
    t = Thread.new { Thread.current.inspect }
    t.value.should include('run')
  end
  
  it "describes a sleeping thread" do
    c = Channel.new
    t = Thread.new do      
      sleep
      c << Thread.current.inspect
      sleep
    end
    
    Thread.pass until t.status == 'sleep'
    t.inspect.should include('sleep')
    t.run
    c.receive.should include('run')
    Thread.pass until t.status == 'sleep'
    t.inspect.should include('sleep')
    t.run
    t.join
    t.inspect.should include('dead')
  end

  compliant_on(:ruby) do
    it "reports aborting on a killed thread" do
      c = Channel.new
      t = Thread.new { c << Thread.current.inspect; Thread.stop }
      c.receive.should include('run')
      t.inspect.should include('sleep')
      Thread.critical = true
      t.kill
      t.inspect.should include('aborting')
      Thread.critical = false
    end
  end
end
