require File.expand_path('../../../spec_helper', __FILE__)

with_feature :fiber do
  describe "Fiber#stack_size" do
    it "returns the Fiber's stack size" do
      f = Fiber.new { }
      f.stack_size.should be_an_instance_of(Fixnum)
    end
  end
end
