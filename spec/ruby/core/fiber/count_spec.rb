require File.expand_path('../../../spec_helper', __FILE__)

describe "Fiber.count" do
  it "returns the number of Fibers that would be returned by .list" do
    Fiber.count.should == Fiber.list.count
  end
end
