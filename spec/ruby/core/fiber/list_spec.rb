require File.expand_path('../../../spec_helper', __FILE__)

describe "Fiber.list" do
  it "returns an Array" do
    Fiber.list.should be_an_instance_of(Array)
  end

  it "returns non-dead Fibers" do
    f1 = Fiber.new { }
    f2 = Fiber.new { }

    f2.resume

    Fiber.list.should include(f1)
    Fiber.list.should_not include(f2)

    f1.resume
  end
end
