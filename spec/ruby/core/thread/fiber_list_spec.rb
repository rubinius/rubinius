require File.expand_path('../../../spec_helper', __FILE__)

describe "Thread.fiber_list" do
  it "returns an Array" do
    Thread.current.fiber_list.should be_an_instance_of(Array)
  end

  it "returns Fibers only for the Thread" do
    start = false

    t1 = Thread.new do
      Thread.pass until start

      f1 = Fiber.new {}
      f2 = Fiber.new {}

      t1.fiber_list.size.should == 2

      f1.resume
      f2.resume
    end

    t2 = Thread.new do
      Thread.pass until start

      t2.fiber_list.size.should == 0
    end

    start = true
    t1.join
    t2.join
  end
end
