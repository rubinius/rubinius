require File.dirname(__FILE__) + '/../../spec_helper'
require 'ostruct'

describe "OpenStruct.delete_field" do
  before :each do
    @os = OpenStruct.new(:name => "John Smith", :age => 70, :pension => 300)
  end
  
  it "removes a named field" do
    @os.name.should == "John Smith"
    @os.delete_field(:name)
    @os.name.should be_nil
  end
end
