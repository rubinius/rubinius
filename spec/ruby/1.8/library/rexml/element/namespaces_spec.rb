require 'rexml/document'
require File.dirname(__FILE__) + '/../../../spec_helper'

describe "REXML::Element#namespace" do
  before :each do
    doc = REXML::Document.new("<a xmlns='1' xmlns:y='2'><b/><c xmlns:z='3'/></a>")
    @elem = doc.elements["//c"]
  end

  it "returns a hash of the namespaces" do
    ns = {"y"=>"2", "z"=>"3", "xmlns"=>"1"}
    @elem.namespaces.keys.should == ns.keys
    @elem.namespaces.values.should == ns.values
  end

  it "returns an empty hash if no namespaces exist" do
    e = REXML::Element.new "element"
    e.namespaces.kind_of?(Hash).should == true
    e.namespaces.should be_empty
  end

  it "uses namespace prefixes as keys" do
    prefixes = ["y", "z", "xmlns"]
    @elem.namespaces.keys.should == prefixes
  end

  it "uses namespace values as the hash values" do
    values = ["2", "3", "1"]
    @elem.namespaces.values.should == values
  end
  
end
