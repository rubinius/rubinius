require File.dirname(__FILE__) + '/../../spec_helper'
require "ostruct"

describe "OpenStruct#initialize_copy" do
  it "is private" do
    OpenStruct.should have_private_instance_method(:initialize_copy)
  end
  
  it "creates an independent method/value table for self" do
    # Yes, I'm using #dup here...
    os = OpenStruct.new("age" => 20, "name" => "John")
    dup = os.dup
    
    dup.age = 30
    dup.age.should eql(30)
    os.age.should eql(20)
  end
end
