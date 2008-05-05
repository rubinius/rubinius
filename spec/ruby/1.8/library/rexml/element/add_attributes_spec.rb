require 'rexml/document'
require File.dirname(__FILE__) + '/../../../spec_helper'

describe "REXML::Element#add_attribute" do
  before :each do
    @person = REXML::Element.new "person"
    @person.attributes["name"] = "Bill"
  end

  it "adds multiple attributes to element" do
    @person.add_attributes({"name" => "Joe", "age" => "27"})
    @person.attributes["name"].should == "Joe"
    @person.attributes["age"].should == "27"
  end

  it "accepts an array of arguments" do
    attrs =  { "name" => "Joe", "age" => "27"}
    @person.add_attributes attrs.to_a
    @person.attributes["name"].should == "Joe"
    @person.attributes["age"].should == "27"
  end

end
