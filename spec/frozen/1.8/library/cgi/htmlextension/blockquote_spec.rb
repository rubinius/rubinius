require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#blockquote when passed a String" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a 'blockquote'-element, using the passed String for the 'cite'-attribute" do
    expected = '<BLOCKQUOTE CITE="http://www.example.com/quotes/foo.html"></BLOCKQUOTE>'
    @html.blockquote("http://www.example.com/quotes/foo.html").should == expected
  end

  it "includes the passed block's return value when passed a block" do
    expected = '<BLOCKQUOTE CITE="http://www.example.com/quotes/foo.html">Foo!</BLOCKQUOTE>'
    @html.blockquote("http://www.example.com/quotes/foo.html") { "Foo!" }.should == expected
  end
end

describe "CGI::HtmlExtension#blockquote when passed a Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a 'blockquote'-element, using the passed Hash for attributes" do
    expected = '<BLOCKQUOTE CITE="http://www.example.com/quotes/foo.html" ID="test"></BLOCKQUOTE>'
    @html.blockquote("CITE" => "http://www.example.com/quotes/foo.html", "ID" => "test").should == expected
  end
  
  it "includes the passed block's return value when passed a block" do
    expected = '<BLOCKQUOTE CITE="http://www.example.com/quotes/foo.html" ID="test">Foo!</BLOCKQUOTE>'
    @html.blockquote("CITE" => "http://www.example.com/quotes/foo.html", "ID" => "test") { "Foo!" }.should == expected
  end
end