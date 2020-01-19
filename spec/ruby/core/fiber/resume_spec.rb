require File.expand_path('../../../spec_helper', __FILE__)

with_feature :fiber do
  describe "Fiber#resume" do
    it "can be invoked from the root Fiber" do
     fiber = Fiber.new { :fiber }
     fiber.resume.should == :fiber
    end

    it "raises a FiberError if invoked from a different Thread" do
      fiber = Fiber.new { }
      lambda do
        Thread.new do
          fiber.resume
        end.join
      end.should raise_error(FiberError)
      fiber.resume
    end

    it "returns control to the calling Fiber if called from one" do
      fiber1 = Fiber.new { :fiber1 }
      fiber2 = Fiber.new { fiber1.resume; :fiber2 }
      fiber2.resume.should == :fiber2
    end

    it "passes control to the beginning of the block on first invocation" do
      invoked = false
      fiber = Fiber.new { invoked = true }
      fiber.resume
      invoked.should be_true
    end

    it "returns the last value encountered on first invocation" do
      fiber = Fiber.new { false; true }
      fiber.resume.should be_true
    end

    it "runs until the end of the block" do
      obj = mock('obj')
      obj.should_receive(:do).once
      fiber = Fiber.new { 1 + 2; a = "glark"; obj.do }
      fiber.resume
    end

    it "runs until Fiber.yield" do
      obj = mock('obj')
      obj.should_not_receive(:do)
      fiber = Fiber.new { 1 + 2; Fiber.yield; obj.do }
      fiber.resume
    end

    it "resumes from the last call to Fiber.yield on subsequent invocations" do
      fiber = Fiber.new { Fiber.yield :first; :second }
      fiber.resume.should == :first
      fiber.resume.should == :second
    end

    it "accepts any number of arguments" do
      fiber = Fiber.new { |a| }
      lambda { fiber.resume(*(1..10).to_a) }.should_not raise_error
    end

    it "sets the block parameters to its arguments on the first invocation" do
      first = mock('first')
      first.should_receive(:arg).with(:first).twice
      fiber = Fiber.new { |arg| first.arg arg; Fiber.yield; first.arg arg; }
      fiber.resume(:first)
      fiber.resume(:second)
    end

    it "raises a FiberError if the Fiber is dead" do
      fiber = Fiber.new { true }
      fiber.resume
      lambda { fiber.resume }.should raise_error(FiberError)
    end

    it "raises a LocalJumpError if the block includes a return statement" do
      fiber = Fiber.new { return; }
      lambda { fiber.resume }.should raise_error(LocalJumpError)
    end

    it "raises a LocalJumpError if the block includes a break statement" do
      fiber = Fiber.new { break; }
      lambda { fiber.resume }.should raise_error(LocalJumpError)
    end
  end

  describe "Fiber#resume with ensure clause" do
    before do
      ScratchPad.record []
    end

    # ruby_bug "redmine #595", "2.1.0"
    it "executes the ensure clause only when control exits the begin clause" do
      f = Fiber.new do
        begin
          ScratchPad << :before_yield
          Fiber.yield
          ScratchPad << :after_yield
        ensure
          ScratchPad << :in_ensure
        end
      end

      # The apparent issue is that when Fiber.yield executes, control
      # "leaves" the "ensure block" and so the ensure clause should run. But
      # control really does NOT leave the ensure block when Fiber.yield
      # executes. It merely pauses there. To require ensure to run when a
      # Fiber is suspended then makes ensure-in-a-Fiber-context different
      # than ensure-in-a-Thread-context and this would be very confusing.
      f.resume

      ScratchPad.recorded.should == [:before_yield]

      # When we execute the second #resume call, the ensure block DOES exit,
      # the ensure clause runs. This is Ruby behavior as of 2.3.1.
      f.resume

      ScratchPad.recorded.should == [:before_yield, :after_yield, :in_ensure]
    end
  end
end
