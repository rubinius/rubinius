require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#inspect" do
  it "can check it's own status" do
    t = Thread.new { Thread.current.inspect }
    t.value.should include('run')
  end
  
  it "should describe a sleeping thread" do
    c = Channel.new
    t = Thread.new do      
      c << :s1      
      sleep
      c << Thread.current.inspect
      sleep
    end
    
    c.receive.should == :s1
    t.inspect.should include('sleep')
    t.run
    c.receive.should include('run')
    t.inspect.should include('sleep')
    t.run
    t.join
    t.inspect.should include('dead')
  end

  compliant(:mri) do
    it "should report aborting on a killed thread" do
      c = Channel.new
      t = Thread.new { c << Thread.current.inspect; Thread.stop }
      c.receive.should include('run')
      t.inspect.should  include('sleep')
      Thread.critical = true
      t.kill
      t.inspect.should include('aborting')
      Thread.critical = false
    end
  end
end
