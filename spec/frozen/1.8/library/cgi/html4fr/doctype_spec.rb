require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Html4Fr#doctype" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4Fr)
  end
  
  it "returns the doctype declaration for the Frameset version of HTML4" do
    @html.doctype.should == '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">'
  end
end
