require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::Html3#doctype" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html3)
  end
  
  it "returns the doctype declaration for HTML3" do
    @html.doctype.should == '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">'
  end
end
