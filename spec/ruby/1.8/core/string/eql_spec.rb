require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'
require File.dirname(__FILE__) + '/shared/equal_value.rb'

describe "String#eql?" do
  it_behaves_like(:string_equal_value, :eql?)
end

describe "String#eql? when given a non-String" do
  it "returns false" do
    'hello'.eql?(5).should == false
    'hello'.eql?(:hello).should == false
    'hello'.eql?(mock('x')).should == false
  end
  
  it "does not try to call #to_str on the given argument" do
    (obj = mock('x')).should_not_receive(:to_str)
    'hello'.eql?(obj).should == false
  end
end