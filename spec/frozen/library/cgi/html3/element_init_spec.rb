require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Html3#element_init" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html3)
  end

  it "initializes the HTML Generation methods for HTML3" do
    @html.respond_to?("a").should be_false
    @html.element_init
    @html.respond_to?("a").should be_true
    
    @html.a.should == "<A></A>"
    @html.a { "link text" }.should == "<A>link text</A>"
    
    @html.br.should == "<BR>"
    
    @html.html.should == "<HTML>"
    @html.html { "html body" }.should == "<HTML>html body</HTML>"
  end

  it "should extend self with CGI::TagMaker" do
    @html.should_not be_kind_of(CGI::TagMaker)
    @html.element_init
    @html.should be_kind_of(CGI::TagMaker)
  end
end
