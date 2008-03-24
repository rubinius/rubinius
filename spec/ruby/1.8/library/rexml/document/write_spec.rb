require 'rexml/document'
require File.dirname(__FILE__) + '/../../../spec_helper'

# Maybe this can be cleaned
describe "REXML::Document#write" do
  before :each do
    @d      = REXML::Document.new
    city    = REXML::Element.new "Springfield"
    street  = REXML::Element.new "EvergreenTerrace"
    address = REXML::Element.new "House742"
    @d << city << street << address
    @str = ""
  end

  it "returns document source as string" do
    @d.write(@str)
    @str.should == "<Springfield><EvergreenTerrace><House742/></EvergreenTerrace></Springfield>"
  end

  it "returns document indented" do 
    @d.write(@str, 2) 
    @str.should == "    <Springfield>\n      <EvergreenTerrace>\n        <House742/>\n      </EvergreenTerrace>\n    </Springfield>"
  end

  it "returns document with transitive support" do
    @d.write(@str, 2, true)
    @str.should ==  "    <Springfield\n      ><EvergreenTerrace\n        ><House742\n        /></EvergreenTerrace\n    ></Springfield\n  >"
  end

  it "returns document with support for IE" do
    @d.write(@str, -1, false, true)
    @str.should ==  "<Springfield><EvergreenTerrace><House742 /></EvergreenTerrace></Springfield>"
  end
end
