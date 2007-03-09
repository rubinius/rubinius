require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should include(expected)" do
  specify "should pass if target includes expected" do
    [1,2,3].should include(3)
    "abc".should include("a")
  end

  specify "should fail if target does not include expected" do
    lambda {
      [1,2,3].should include(4)
    }.should fail_with("expected [1, 2, 3] to include 4")
    lambda {
      "abc".should include("d")
    }.should fail_with("expected \"abc\" to include \"d\"")
  end
end

context "should_not include(expected)" do
  specify "should pass if target does not include expected" do
    [1,2,3].should_not include(4)
    "abc".should_not include("d")
  end

  specify "should fail if target includes expected" do
    lambda {
      [1,2,3].should_not include(3)
    }.should fail_with("expected [1, 2, 3] not to include 3")
    lambda {
      "abc".should_not include("c")
    }.should fail_with("expected \"abc\" not to include \"c\"")
  end
end
