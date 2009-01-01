require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Html4Fr#element_init" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    
    @html.extend(CGI::Html4Fr)
  end

  it "initializes the HTML Generation methods for the Frameset version of HTML4" do
    @html.respond_to?("frameset").should be_false
    @html.element_init
    @html.respond_to?("frameset").should be_true
    
    @html.frameset.should == "<FRAMESET></FRAMESET>"
    @html.frameset { "link text" }.should == "<FRAMESET>link text</FRAMESET>"
    
    @html.frame.should == "<FRAME>"
  end
end
