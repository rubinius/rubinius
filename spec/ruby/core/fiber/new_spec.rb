require File.expand_path('../../../spec_helper', __FILE__)

with_feature :fiber do
  describe "Fiber.new" do
    it "accepts an optional keyword argument to set the Fiber's stack size" do
      size = 81920
      f = Fiber.new(stack_size: size) { }
      f.resume
      f.stack_size.should == size
    end

    it "calls #to_int to covert the stack size to a Fixnum" do
      size = mock("Fiber stack size")
      size.should_receive(:to_int).and_return(81920)

      f = Fiber.new(stack_size: size) { }
      f.resume
      f.stack_size.should == 81920
    end

    it "creates a fiber from the given block" do
      fiber = Fiber.new {}
      fiber.resume
      fiber.should be_an_instance_of(Fiber)
    end

    it "creates a fiber from a subclass" do
      class MyFiber < Fiber
      end
      fiber = MyFiber.new {}
      fiber.resume
      fiber.should be_an_instance_of(MyFiber)
    end

    it "raises an ArgumentError if called without a block" do
      lambda { Fiber.new }.should raise_error(ArgumentError)
    end

    it "does not invoke the block" do
      invoked = false
      fiber = Fiber.new { invoked = true }
      invoked.should be_false
      fiber.resume
    end

    it "closes over lexical environments" do
      o = Object.new
      def o.f
        a = 1
        f = Fiber.new { a = 2 }
        f.resume
        a
      end
      o.f.should == 2
    end
  end
end
