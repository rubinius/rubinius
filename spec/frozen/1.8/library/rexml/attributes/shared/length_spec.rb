require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'rexml/document'

shared :length do |cmd|
  describe "REXML::Attributes##{cmd}" do
    it "returns the number of attributes" do
      e = REXML::Element.new("root")
      e.attributes.length.should == 0

      e.attributes << REXML::Attribute.new("name", "John")
      e.attributes << REXML::Attribute.new("another_name", "Leo")
      e.attributes.length.should == 2
    end
  end
end

describe "REXML::Attributes#length" do
 it_behaves_like(:length, "Attributes", :length)
end

describe "REXML::Attributes#size" do
 it_behaves_like(:length, "Attributes", :size)
end
