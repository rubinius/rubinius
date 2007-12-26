require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "String#substring" do
    specify "returns the portion of string specified by index, length" do
      "blah".substring(0, 2).should == "bl"
      "blah".substring(0, 4).should == "blah"
      "blah".substring(2, 2).should == "ah"
    end
  end
end
