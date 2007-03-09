require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "should throw_symbol" do
  specify "should pass if any Symbol is thrown" do
    lambda{ throw :sym }.should throw_symbol
  end

  specify "should fail if no Symbol is thrown" do
    lambda {
      lambda { }.should throw_symbol
    }.should fail_with("expected a Symbol but nothing was thrown")
  end
end

context "should_not throw_symbol" do
  specify "should pass if no Symbol is thrown" do
    lambda{ }.should_not throw_symbol
  end

  specify "should fail if any Symbol is thrown" do
    lambda {
      lambda { throw :sym }.should_not throw_symbol
    }.should fail_with("expected no Symbol, got :sym")
  end
end

context "should throw_symbol(:sym)" do
  specify "should pass if correct Symbol is thrown" do
    lambda{ throw :sym }.should throw_symbol(:sym)
  end

  specify "should fail if no Symbol is thrown" do
    lambda {
      lambda { }.should throw_symbol(:sym)
    }.should fail_with("expected :sym but nothing was thrown")
  end

  specify "should fail if wrong Symbol is thrown" do
    lambda {
      lambda { throw :wrong_sym }.should throw_symbol(:sym)
    }.should fail_with("expected :sym, got :wrong_sym")
  end
end

context "should_not throw_symbol(:sym)" do
  specify "should pass if no Symbol is thrown" do
    lambda { }.should_not throw_symbol(:sym)
  end

  specify "should pass if other Symbol is thrown" do
    lambda { throw :wrong_sym }.should_not throw_symbol(:sym)
  end

  specify "should fail if correct Symbol is thrown" do
    lambda {
      lambda{ throw :sym }.should_not throw_symbol(:sym)
    }.should fail_with("expected :sym not to be thrown")
  end
end
