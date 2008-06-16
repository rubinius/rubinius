require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'rexml/document'

shared :to_s do |cmd|
  describe "REXML::CData##{cmd}" do
    it "returns the contents of the CData" do
      c = REXML::CData.new("some text")
      c.to_s.should == "some text"
    end
    
    it "does not escape text" do
      c1 = REXML::CData.new("some& text\n")
      c1.value.should == "some& text\n"
    end
  end
end

describe "REXML::CData#to_s" do
  it_behaves_like(:to_s, "CData", :to_s)
end

describe "REXML::CData#value" do
  it_behaves_like(:to_s, "CData", :value)
end

