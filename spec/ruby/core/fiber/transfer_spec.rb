require File.expand_path('../../../spec_helper', __FILE__)

with_feature :fiber do
  describe "Fiber#transfer" do
    it "can be invoked from the root Fiber" do
     fiber = Fiber.new { :fiber }
     fiber.transfer.should == :fiber
    end

    it "returns to the root Fiber when finished" do
      f1 = Fiber.new { :fiber_1 }
      f2 = Fiber.new { f1.transfer; :fiber_2 }

      f2.transfer.should == :fiber_1
      f2.transfer.should == :fiber_2
    end

    it "raises a FiberError if invoked from a different Thread" do
      fiber = Fiber.new { }
      lambda do
        Thread.new do
          fiber.transfer
        end.join
      end.should raise_error(FiberError)
      fiber.resume
    end
  end
end
