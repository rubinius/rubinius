require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Proc#lambda?" do
    it "returns true if the object was created with the lambda keyword" do
      lambda { true }.lambda?.should be_true
    end

    it "returns false if the object was created with the proc keyword" do
      proc { true }.lambda?.should be_false
    end

    it "returns false if the object was created with Proc.new" do
      Proc.new { true }.lambda?.should be_false
    end

    # [ruby-core:24127]
    it "returns true if the object was created with Proc#curry" do
      l = lambda { true }
      l.curry.lambda?.should be_true
    end
  end
end
