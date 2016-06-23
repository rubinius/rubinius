require File.expand_path('../../../spec_helper', __FILE__)

describe "Fiber.list" do
  it "returns the root Fiber for the thread" do
    Fiber.main.should be_an_instance_of(Fiber)
  end
end
