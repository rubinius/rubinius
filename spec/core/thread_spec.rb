require File.dirname(__FILE__) + '/../spec_helper'

unless defined? Channel
  require 'thread'
  class Channel < Queue
    alias receive shift
  end
end

describe "Thread.new" do
  it "creates a thread executing the given block" do
    c = Channel.new
    Thread.new { c << true }.join
    c << false
    c.receive.should == true
  end
end

describe "Thread.current" do
  it "returns a thread" do
    current = Thread.current
    current.class.should == Thread
  end

  it "returns the current thread" do
    t = Thread.new { Thread.current }
    t.value.should.equal? t
  end
end

describe "Thread.main" do
  it "returns the main thread" do
    Thread.new { @main = Thread.main ; @current = Thread.current}.join
    @main.should_not == @current
  end
end

describe "Thread#value" do
  it "returns the result of the block" do
    Thread.new { 3 }.value.should == 3
  end
end

describe "Thread#alive?" do
  it "returns true as long as the thread is alive" do
    c = Channel.new
    t = Thread.new { c.receive }
    begin
      t.alive?.should == true
    ensure
      c << nil
    end
  end

  it "returns false when the thread is finished" do
    t = Thread.new {}
    t.join
    t.alive?.should == false
  end
end

describe "Thread#join" do
  it "returns the thread when it is finished" do
    t = Thread.new {}
    t.join.should.equal? t
  end

  it "returns the thread when it is finished when given a timeout" do
    t = Thread.new {}
    t.join
    t.join(0).should.equal? t
  end

  it "returns nil if it is not finished when given a timeout" do
    c = Channel.new
    t = Thread.new { c.receive }
    begin
      t.join(0).should == nil
    ensure
      c << true
    end
  end

  it "raises any exceptions encountered in the thread body" do
    t = Thread.new { raise NotImplementedError.new("Just kidding") }
    should_raise(NotImplementedError) { t.join }
  end
end

describe "Thread#status" do
  it "reports the thread's current state" do
    v = nil
    t = Thread.new {
      v = Thread.current.status
    }
    t.join
    v.should == 'run'
    t.status.should == false
  end

end

describe "Thread#inspect" do
  it "shows the thread's status" do
    x = nil
    t = Thread.new { x = Thread.current.inspect; sleep 4 }
    Thread.pass until t.status == false || t.status == 'sleep'
    x.include?('run').should == true
    t.inspect.include?('sleep').should == true
    t.kill
    Thread.pass while t.alive?
    t.inspect.include?('dead').should == true
  end
end
