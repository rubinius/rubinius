require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/../../shared/kernel/raise'

describe "Thread#raise" do
  it "ignores dead threads" do
    t = Thread.new { :dead }
    Thread.pass while t.alive?
    lambda {t.raise("Kill the thread")}.should_not raise_error
    lambda {t.value}.should_not raise_error
  end
end

describe "Thread#raise on a sleeping thread" do
  before :each do
    ScratchPad.clear
    @thr = ThreadSpecs.sleeping_thread
    Thread.pass while @thr.status and @thr.status != "sleep"
  end

  after :each do
    @thr.kill
  end

  it "raises a RuntimeError if no exception class is given" do
    @thr.raise
    Thread.pass while @thr.status
    ScratchPad.recorded.should be_kind_of(RuntimeError)
  end

  it "raises the given exception" do
    @thr.raise Exception
    Thread.pass while @thr.status
    ScratchPad.recorded.class.should == Exception
  end

  it "raises the given exception with the given message" do
    @thr.raise Exception, "get to work"
    Thread.pass while @thr.status
    ScratchPad.recorded.class.should == Exception
    ScratchPad.recorded.message.should == "get to work"
  end

  it "can go unhandled" do
    t = Thread.new do
      sleep
    end

    t.raise
    lambda {t.value}.should raise_error(RuntimeError)
  end

  it "re-raises active exception" do
    t = Thread.new do
      begin
        1/0
      rescue ZeroDivisionError
        sleep 3
      end
    end

    Thread.pass while t.status and t.status != "sleep"
    t.raise
    lambda {t.value}.should raise_error(ZeroDivisionError)
    t.kill
  end
end

describe "Thread#raise on a running thread" do
  before :each do
    ScratchPad.clear
    @thr = ThreadSpecs.running_thread
    Thread.pass while @thr.status and @thr.status != "run"
  end
  
  after :each do
    @thr.kill
  end

  it "raises a RuntimeError if no exception class is given" do
    @thr.raise
    Thread.pass while @thr.status
    ScratchPad.recorded.should be_kind_of(RuntimeError)
  end

  it "raises the given exception" do
    @thr.raise Exception
    Thread.pass while @thr.status
    ScratchPad.recorded.class.should == Exception
  end

  it "raises the given exception with the given message" do
    @thr.raise Exception, "get to work"
    Thread.pass while @thr.status
    ScratchPad.recorded.class.should == Exception
    ScratchPad.recorded.message.should == "get to work"
  end

  it "can go unhandled" do
    t = Thread.new do
      loop {}
    end

    t.raise
    lambda {t.value}.should raise_error(RuntimeError)
  end

  it "re-raises active exception" do
    raised = false
    t = Thread.new do
      begin
        1/0
      rescue ZeroDivisionError
        raised = true
        loop { }
      end
    end

    Thread.pass until raised || !t.alive?
    t.raise
    lambda {t.value}.should raise_error(ZeroDivisionError)
  end
end

describe "Thread#raise on same thread" do
  it_behaves_like :kernel_raise, :raise, Thread.current
end
