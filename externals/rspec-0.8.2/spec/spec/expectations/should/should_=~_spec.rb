require File.dirname(__FILE__) + '/../../../spec_helper'

context "should =~" do
  specify "should pass when =~ operator returns non-nil" do
    "foo".should =~ /oo/
  end

  specify "should fail when =~ operator returns nil" do
    lambda do
      "fu".should =~ /oo/
    end.should_fail_with "expected =~ /oo/, got \"fu\""
  end
end
