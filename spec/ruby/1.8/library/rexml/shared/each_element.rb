require 'rexml/document'
require File.dirname(__FILE__) + '/../../../spec_helper'
 
shared :each_element do |klass, cmd|
  describe "REXML::#{klass}##{cmd}" do
    before :each do
      @e = REXML::Element.new "root"
      s1 = REXML::Element.new "node1"
      s2 = REXML::Element.new "node2"
      s3 = REXML::Element.new "node3"
      s4 = REXML::Element.new "sub_node"
      @e << s1 
      @e << s2
      @e << s3
      @e << s4
    end
 
    it "iterates through element" do
      str = ""
        @e.each_element { |elem| str << elem.name << " " }
      str.should == "node1 node2 node3 sub_node "
    end
 
    it "iterates through element filtering with XPath" do
      str = ""
       @e.each_element("/*"){ |e| str << e.name << " "}
       str.should == "node1 node2 node3 sub_node "
    end
  end
end
 
describe "REXML::Element#each_element" do
 it_behaves_like(:each_element, "Element", :each_element)
end
 
describe "REXML::Elements#each" do
  it_behaves_like(:each_element, "Elements", :each)
end
