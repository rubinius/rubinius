require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "NilClass#rationalize" do
    it "returns 0/1" do
      nil.rationalize.should == Rational(0, 1)
    end

    it "ignores arguments" do
      nil.rationalize(0.1).should == Rational(0, 1)
    end
  end
end
