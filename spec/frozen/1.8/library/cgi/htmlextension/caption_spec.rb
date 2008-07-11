require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#caption when passed a String" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a 'caption'-element, using the passed String for the 'align'-attribute" do
    expected = '<CAPTION ALIGN="left"></CAPTION>'
    @html.caption("left").should == expected
  end
  
  it "includes the passed block's return value when passed a block" do
    expected = '<CAPTION ALIGN="left">Capital Cities</CAPTION>'
    @html.caption("left") { "Capital Cities" }.should == expected
  end
end

describe "CGI::HtmlExtension#caption when passed a Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a 'caption'-element, using the passed Hash for attributes" do
    expected = '<CAPTION ID="test" ALIGN="left"></CAPTION>'
    @html.caption("ALIGN" => "left", "ID" => "test").should == expected
  end
  
  it "includes the passed block's return value when passed a block" do
    expected = '<CAPTION ID="test" ALIGN="left">Capital Cities</CAPTION>'
    @html.caption("ALIGN" => "left", "ID" => "test") { "Capital Cities" }.should == expected
  end
end
