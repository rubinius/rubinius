require File.expand_path('../../../spec_helper', __FILE__)

describe "Object#become" do
  it "replaces the receiver with the argument bidirectionally" do
    a, b = "me", "you"

    a.should eql("me")
    b.should eql("you")

    a.become b

    a.should eql("you")
    b.should eql("me")
  end
end
