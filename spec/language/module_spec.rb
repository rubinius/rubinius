require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/private'

describe "The private keyword" do

  it "should mark a method private" do
    Private::D.private_instance_methods.should_include("foo")
    Private::E.new.respond_to?("bar").should == false
  end

end
