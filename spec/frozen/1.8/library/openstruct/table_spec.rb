require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'

describe "OpenStruct.marshal_dump" do
  before :each do
    @os = OpenStruct.new(:name => "John Smith", :age => 30)
  end
  
  it "returns a Hash object" do
    @os.marshal_dump.should be_kind_of(Hash)
  end
  
  it "returns a Hash object with two keys" do
    @os.marshal_dump.keys.size.should == 2
  end
end