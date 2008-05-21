require 'rexml/document'
require 'rexml/formatters/transitive'
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
    @str.should =~ /\s*<Springfield>\s*<EvergreenTerrace>\s*<House742\/>\s*<\/EvergreenTerrace>\s*<\/Springfield>/
  end

  # REXML in p114 is screwed up:
  # Document#write uses wrong arity for Formatters::Transitive#initialize
  #
  # In branch_1_8 in rev 15833 REXML is organized completely differently.
  # So we are waiting for further changes to REXML in 1.8.x branch.
  ruby_bug "#", "1.8.6.114" do
    it "returns document with transitive support" do
      @d.write(@str, 2, true)
      @str.should =~  "\s*<Springfield\s*><EvergreenTerrace\s*><House742\s*\/><\/EvergreenTerrace\s*><\/Springfield\s*>"
    end
  end

  it "returns document with support for IE" do
    @d.write(@str, -1, false, true)
    @str.should ==  "<Springfield><EvergreenTerrace><House742 /></EvergreenTerrace></Springfield>"
  end
end
