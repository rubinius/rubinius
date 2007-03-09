require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_be_a_kind_of" do
  specify "should fail when target is not specified class" do
    lambda do
      5.should_be_a_kind_of(String)
    end.should_fail_with "expected kind_of?(String) to return true, got false"
  end

  specify "should pass when target is of specified class" do
    lambda do
      5.should_be_a_kind_of(Fixnum)
    end.should_pass
  end

  specify "should pass when target is of subclass of specified class" do
    lambda do
      5.should_be_a_kind_of(Integer)
    end.should_pass
  end
end
