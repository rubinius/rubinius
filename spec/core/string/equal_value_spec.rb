require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'
require File.dirname(__FILE__) + '/shared/equal_value.rb'

describe "String#==" do
  it_behaves_like(:string_equal_value, :==)
end

describe "String#==" do
  it "returns false if obj does not respond to to_str" do
    ('hello' == 5).should == false
    ('hello' == :hello).should == false
    ('hello' == Object.new).should == false
  end
  
  it "returns obj == self if obj responds to to_str" do
    obj = Object.new
    def obj.to_str() 'world!' end
    obj.should_receive(:==).any_number_of_times.and_return(true)
    ('hello' == obj).should == true
    ('world!' == obj).should == true 
    
    obj = Object.new
    other = "abc"
    obj.should_receive(:respond_to?).with(:to_str).and_return(true)
    obj.should_receive(:==).and_return(true)
    (other == obj).should == true
  end
end