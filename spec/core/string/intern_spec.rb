require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#intern" do
  it "returns a symbol of itself" do
    "hello there".intern.should == :"hello there"
    "hello".intern.should == :hello
  end
end