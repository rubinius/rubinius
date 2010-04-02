require File.dirname(__FILE__) + '/spec_helper'

load_extension("thread")

describe "CApiThreadSpecs" do
  before :each do
    @t = CApiThreadSpecs.new
    ScratchPad.clear
  end

  describe "rb_thread_select" do
    it "returns true if an fd is ready to read" do
      read, write = IO.pipe

      @t.rb_thread_select_fd(read.to_i, 0).should == false
      write << "1"
      @t.rb_thread_select_fd(read.to_i, 0).should == true
    end

    it "does not block all threads" do
      t = Thread.new do
        sleep 0.25
        ScratchPad.record :inner
      end
      Thread.pass while t.status and t.status != "sleep"

      @t.rb_thread_select(500_000)

      t.alive?.should be_false
      ScratchPad.recorded.should == :inner

      t.join
    end
  end

  describe "rb_thread_alone" do
    it "returns 0" do
      @t.rb_thread_alone.should == 0
    end
  end

  describe "rb_thread_current" do
    it "equals Thread.current" do
      @t.rb_thread_current.should == Thread.current
    end
  end
end
