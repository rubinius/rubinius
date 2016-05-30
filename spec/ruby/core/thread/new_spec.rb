require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Thread.new" do
  it "accepts an optional keyword argument to set the Thread's stack size" do
    size = 81920
    t = Thread.new(stack_size: size) { }
    t.stack_size.should == size
  end

  it "calls #to_int to covert the stack size to a Fixnum" do
    size = mock("Thread stack size")
    size.should_receive(:to_int).and_return(81920)

    t = Thread.new(stack_size: size) { }
    t.stack_size.should == 81920
  end

  it "creates a thread executing the given block" do
    c = Channel.new
    Thread.new { c << true }.join
    c << false
    c.receive.should == true
  end

  it "can pass arguments to the thread block" do
    arr = []
    a, b, c = 1, 2, 3
    t = Thread.new(a,b,c) {|d,e,f| arr << d << e << f }
    t.join
    arr.should == [a,b,c]
  end

  it "raises an exception when not given a block" do
    lambda { Thread.new }.should raise_error(ThreadError)
  end

  it "creates a subclass of thread calls super with a block in initialize" do
    arr = []
    t = ThreadSpecs::SubThread.new(arr)
    t.join
    arr.should == [1]
  end

  it "calls #initialize and raises an error if super not used" do
    c = Class.new(Thread) do
      def initialize
      end
    end

    lambda {
      c.new
    }.should raise_error(ThreadError)
  end

  it "calls and respects #initialize for the block to use" do
    c = Class.new(Thread) do
      def initialize
        ScratchPad.record [:good]
        super { ScratchPad << :in_thread }
      end
    end

    t = c.new
    t.join

    ScratchPad.recorded.should == [:good, :in_thread]
  end

end
