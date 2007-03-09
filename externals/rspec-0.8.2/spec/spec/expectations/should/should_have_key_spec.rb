require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_have_key" do
  specify "should pass when key is present" do
    lambda do
      {"a" => 1}.should_have_key("a")
    end.should_not_raise
  end

  specify "should fail when key is not present" do
    lambda do
      {"a" => 1}.should_have_key("b")
    end.should_fail_with('expected #has_key?("b") to return true, got false')
  end

  specify "should fail when target does not respond to has_key?" do
    lambda do
      Object.new.should_have_key("b")
    end.should_fail_with /target does not respond to #has_key\?/
  end
end
