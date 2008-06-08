require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'

describe "OpenStruct#to_s" do
  before :each do
    @os = OpenStruct.new(:name => "John Smith")
  end
  
  it "returns a string representation of the OpenStruct object" do
    @os.inspect.should be_kind_of(String)
    @os.to_s.should == "#<OpenStruct name=\"John Smith\">"
  end
end