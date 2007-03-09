require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should respond_to(:sym)" do
  
  specify "should pass if target responds to :sym" do
    Object.new.should respond_to(:methods)
  end
  
  specify "should fail target does not respond to :sym" do
    lambda {
      Object.new.should respond_to(:some_method)
    }.should fail_with("expected target to respond to :some_method")
  end
  
end

context "should_not respond_to(:sym)" do
  
  specify "should pass if target does not respond to :sym" do
    Object.new.should_not respond_to(:some_method)
  end
  
  specify "should fail target responds to :sym" do
    lambda {
      Object.new.should_not respond_to(:methods)
    }.should fail_with("expected target not to respond to :methods")
  end
  
end

