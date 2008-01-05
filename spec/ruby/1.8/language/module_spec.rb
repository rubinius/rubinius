require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/private'

describe "The private keyword" do
  it "marks a method private" do
    Private::D.private_instance_methods.should include("foo")
    Private::E.new.respond_to?("bar").should == false
  end
end
