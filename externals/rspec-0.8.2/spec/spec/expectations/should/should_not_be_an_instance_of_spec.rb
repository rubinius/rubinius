require File.dirname(__FILE__) + '/../../../spec_helper.rb'

context "should_not_be_an_instance_of" do
  specify "should fail when target is of specified class" do
    lambda { "hello".should_not_be_an_instance_of(String) }.should_fail
  end

  specify "should pass when target is not of specified class" do
    [].should_not_be_an_instance_of(String)      
  end
end