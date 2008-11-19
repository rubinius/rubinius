require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/fixtures/common"

describe "CGI::HtmlExtension#blockquote" do
  before(:each) do
    @html = CGISpecs::HtmlExtension.new
  end
  
  describe "when passed a String" do
    it "returns a 'blockquote'-element, using the passed String for the 'cite'-attribute" do
      output = @html.blockquote("http://www.example.com/quotes/foo.html")
      output.should equal_element("BLOCKQUOTE", "CITE" => "http://www.example.com/quotes/foo.html")
    end

    it "includes the passed block's return value when passed a block" do
      output = @html.blockquote("http://www.example.com/quotes/foo.html") { "Foo!" }
      output.should equal_element("BLOCKQUOTE", { "CITE" => "http://www.example.com/quotes/foo.html" }, "Foo!")
    end
  end
  
  describe "when passed a Hash" do
    it "returns a 'blockquote'-element, using the passed Hash for attributes" do
      output = @html.blockquote("CITE" => "http://www.example.com/quotes/foo.html", "ID" => "test")
      output.should equal_element("BLOCKQUOTE", "CITE" => "http://www.example.com/quotes/foo.html", "ID" => "test")
    end

    it "includes the passed block's return value when passed a block" do
      output = @html.blockquote("CITE" => "http://www.example.com/quotes/foo.html", "ID" => "test") { "Foo!" }
      output.should equal_element("BLOCKQUOTE", {"CITE" => "http://www.example.com/quotes/foo.html", "ID" => "test"}, "Foo!")
    end
  end
end