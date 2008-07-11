require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Html4Tr#doctype" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4Tr)
  end
  
  it "returns the doctype declaration for the Transitional version of HTML4" do
    @html.doctype.should == '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">'
  end
end
