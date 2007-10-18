require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/private'

describe "The private keyword" do
  it "should mark following methods as being private" do
    a = Private::A.new
    a.methods.should_not_include("bar")

    should_raise(NoMethodError) { a.bar }
  end

  it "should wear off when the class is closed" do
    b = Private::B.new
    b.methods.should_include("foo")
    b.foo
  end
end
