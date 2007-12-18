require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#inspect" do
  it "shows the thread's status" do
    x = nil
    t = Thread.new { x = Thread.current.inspect; sleep 4 }
    Thread.pass while t.status == 'run'
    x.include?('run').should == true
    t.inspect.include?('sleep').should == true
    t.kill
    Thread.pass while t.alive?
    t.inspect.include?('dead').should == true
  end
end
