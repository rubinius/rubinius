require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.select" do

  it "returns the Hash for which block return true" do
    ENV["foo"] = "bar"
    ENV.select { |k, v| k == "foo" }.should == [["foo", "bar"]]
    ENV.delete "foo"
  end

  ruby_version_is "" ... "1.8.7" do
    it "raises when no block is given" do
      lambda { ENV.select }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator when no block is given" do
      ENV.select.should be_kind_of(Enumerable::Enumerator)
    end
  end
end
