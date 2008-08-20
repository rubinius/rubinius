require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#a when passed a String" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns an 'a'-element, using the passed String as the 'href'-attribute" do
    expected = '<A HREF="http://www.example.com"></A>'
    @html.a("http://www.example.com").should == expected
  end
  
  it "includes the passed block's return value when passed a block" do
    expected = '<A HREF="http://www.example.com">Example</A>'
    @html.a("http://www.example.com") { "Example" }.should == expected
  end
end

describe "CGI::HtmlExtension#a when passed a Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns an 'a'-element, using the passed Hash for attributes" do
    expected = '<A HREF="http://www.example.com" TARGET="_top"></A>'
    @html.a("HREF" => "http://www.example.com", "TARGET" => "_top").should == expected
  end

  it "includes the passed block's return value when passed a block" do
    expected = '<A HREF="http://www.example.com" TARGET="_top">Example</A>'
    @html.a("HREF" => "http://www.example.com", "TARGET" => "_top") { "Example" }.should == expected
  end
end