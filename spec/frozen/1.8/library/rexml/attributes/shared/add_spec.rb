require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'rexml/document'

shared :add do |cmd|
  describe "REXML::Attributes##{cmd}" do

    before :each do
      @e = REXML::Element.new("root")
      @attr = REXML::Attributes.new(@e)
      @name = REXML::Attribute.new("name", "Joe")
    end

    it "adds an attribute" do
      @attr.add(@name)
      @attr["name"].should == "Joe"
    end

    it "replaces an existing attribute" do
      @attr.add(REXML::Attribute.new("name", "Bruce"))
      @attr["name"].should == "Bruce"
    end

  end
end

describe "REXML::Attributes#add" do
 it_behaves_like(:add, "Attributes", :add)
end

describe "REXML::Attributes#<<" do
 it_behaves_like(:add, "Attributes", :<<)
end



