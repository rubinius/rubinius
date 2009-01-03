require File.dirname(__FILE__) + '/../../spec_helper'
require 'csv'

describe "CSV.generate_line" do

  it "generates an empty string" do
    result = CSV::generate_line([]) 
    result.should == ""
  end
 
  it "generates the string 'foo,bar'" do
    result = CSV::generate_line(["foo", "bar"])
    result.should == "foo,bar"
  end

  it "generates the string 'foo;bar'" do
    result = CSV::generate_line(["foo", "bar"], ?;)
    result.should == "foo;bar"
  end

  it "generates the string 'foo,,bar'" do
    result = CSV::generate_line(["foo", nil, "bar"])
    result.should == "foo,,bar"
  end

  it "generates the string 'foo;;bar'" do
    result = CSV::generate_line(["foo", nil, "bar"], ?;)
    result.should == "foo;;bar"
  end
end
