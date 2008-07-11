require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#base when passed a String" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a 'base'-element, using the passed String as the 'href'-attribute" do
    expected = '<BASE HREF="http://www.example.com">'
    @html.base("http://www.example.com").should == expected
  end

  it "includes the passed block's return value when passed a block" do
    expected = '<BASE HREF="http://www.example.com">'
    @html.base("http://www.example.com") { "Example" }.should == expected
  end
end

describe "CGI::HtmlExtension#base when passed a Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a 'base'-element, using the passed Hash for attributes" do
    expected = '<BASE HREF="http://www.example.com" ID="test">'
    @html.base("HREF" => "http://www.example.com", "ID" => "test").should == expected
  end

  it "includes the passed block's return value when passed a block" do
    expected = '<BASE HREF="http://www.example.com" ID="test">'
    @html.base("HREF" => "http://www.example.com", "ID" => "test") { "Example" }.should == expected
  end
end
