require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Html4#doctype" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
  end
  
  it "returns the doctype declaration for HTML4" do
    @html.doctype.should == '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">'
  end
end
