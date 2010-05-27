require File.expand_path('../spec_helper', __FILE__)

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
    it "returns true if there is only one thread" do
      pred = Thread.list.size == 1
      @t.rb_thread_alone.should == pred
    end
  end

  describe "rb_thread_current" do
    it "equals Thread.current" do
      @t.rb_thread_current.should == Thread.current
    end
  end

  describe "rb_thread_local_aref" do
    it "returns the value of a thread-local variable" do
      thr = Thread.current
      sym = :thread_capi_specs_aref
      thr[sym] = 1
      @t.rb_thread_local_aref(thr, sym).should == 1
    end

    it "returns nil if the value has not been set" do
      @t.rb_thread_local_aref(Thread.current, :thread_capi_specs_undefined).should be_nil
    end
  end

  describe "rb_thread_local_aset" do
    it "sets the value of a thread-local variable" do
      thr = Thread.current
      sym = :thread_capi_specs_aset
      @t.rb_thread_local_aset(thr, sym, 2).should == 2
      thr[sym].should == 2
    end
  end

  extended_on :rubinius do
    describe "rb_thread_blocking_region" do
      it "runs a C function with the global lock unlocked" do
        @t.rb_thread_blocking_region.should be_true
      end
    end
  end
end
