require File.expand_path('../../../spec_helper', __FILE__)

describe "Kernel#itself" do
  it "should return the receiver" do
    num = 2

    num.itself.should = num
  end
end
