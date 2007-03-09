require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_be_a_kind_of" do
  specify "should fail when target is of specified class" do
    lambda do
      5.should_not_be_a_kind_of(Fixnum)
    end.should_fail
  end

  specify "should fail when target is of subclass of specified class" do
    lambda do
      5.should_not_be_a_kind_of(Integer)
    end.should_fail
  end

  specify "should pass when target is not specified class" do
    lambda do
      5.should_not_be_a_kind_of(String)
    end.should_not_raise
  end
end
