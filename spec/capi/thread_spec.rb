require File.dirname(__FILE__) + '/spec_helper'

load_extension("thread")

describe "CApiThreadSpecs" do
  before :each do
    @t = CApiThreadSpecs.new
  end

  it "rb_thread_select tests an fd" do
    read, write = IO.pipe

    @t.sf_read_ready(read.to_i).should == false
    write << "1"
    @t.sf_read_ready(read.to_i).should == true
  end

  it "rb_thread_select doesn't block all threads" do
    val = nil
    thr = Thread.new do
      sleep 1
      val = :inner
    end

    @t.sf_sleep

    thr.alive?.should == false
    val.should == :inner

    thr.join
  end

end
