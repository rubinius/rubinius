require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'

describe "OpenStruct#table" do
  before(:each) do
    @os = OpenStruct.new("age" => 20, "name" => "John")
  end
  
  it "is protected" do
    @os.protected_methods.should include("table")
  end
  
  it "returns self's method/value table" do
    @os.send(:table).should == { :age => 20, :name => "John" }
  end
end
