require File.dirname(__FILE__) + '/../../spec_helper'
require "ostruct"

describe "OpenStruct#new_ostruct_member when passed [method_name]" do
  before(:each) do
    @os = OpenStruct.new
    @os.instance_variable_set(:@table, :age => 20)
  end
  
  it "creates an attribute reader method for the passed method_name" do
    @os.new_ostruct_member(:age)
    @os.age.should eql(20)
  end
  
  it "creates an attribute writer method for the passed method_name" do
    @os.new_ostruct_member(:age)
    @os.age = 30
    @os.age.should eql(30)
  end
  
  it "does not allow overwriting existing methods" do
    def @os.age
      10
    end
    
    @os.new_ostruct_member(:age)
    @os.age.should == 10
    @os.respond_to?(:age=).should be_false
  end
end
