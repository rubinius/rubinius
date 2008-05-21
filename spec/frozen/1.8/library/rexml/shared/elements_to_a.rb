require 'rexml/document'
require File.dirname(__FILE__) + '/../../../spec_helper'

shared :elements_to_a do |klass, cmd| 
  describe "REXML::#{klass}##{cmd}" do
    before :each do
      @e = REXML::Element.new "root"
      @first = REXML::Element.new("FirstChild")
      @second = REXML::Element.new("SecondChild")
      @e << @first
      @e << @second
    end

    it "returns elements that match xpath" do
      @e.elements.to_a("FirstChild").first.should == @first
    end

    # According to the docs REXML::Element#get_elements is an alias for 
    # REXML::Elements.to_a. Implementation wise there's a difference, get_elements
    # always needs the first param (even if it's nil).
    # A patch was submitted:
    # http://rubyforge.org/tracker/index.php?func=detail&aid=19354&group_id=426&atid=1698
    ruby_bug "#", "1.8.6.114" do
      it "returns all childs if xpath is nil" do
        @e.elements.to_a.should == [@first, @second]
      end
    end

  end
end

describe "REXML::REXML::Elements#to_a" do
  it_behaves_like(:elements_to_a, "Elements", :to_a)
end

describe "REXML::REXML::Element#get_elements" do
  it_behaves_like(:elements_to_a, "Element", :get_elements)
end
