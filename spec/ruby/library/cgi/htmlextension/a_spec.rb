require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/fixtures/common"

describe "CGI::HtmlExtension#a" do
  before(:each) do
    @html = CGISpecs::HtmlExtension.new
  end
  
  describe "when passed a String" do
    it "returns an 'a'-element, using the passed String as the 'href'-attribute" do
      output = @html.a("http://www.example.com")
      output.should equal_element("A", "HREF" => "http://www.example.com")
    end
  
    it "includes the passed block's return value when passed a block" do
      output = @html.a("http://www.example.com") { "Example" }
      output.should equal_element("A", { "HREF" => "http://www.example.com" }, "Example")
    end
  end
  
  describe "when passed a Hash" do
    it "returns an 'a'-element, using the passed Hash for attributes" do
      attributes = {"HREF" => "http://www.example.com", "TARGET" => "_top"}
      @html.a(attributes).should equal_element("A", attributes)
    end

    it "includes the passed block's return value when passed a block" do
      attributes = {"HREF" => "http://www.example.com", "TARGET" => "_top"}
      @html.a(attributes) { "Example" }.should equal_element("A", attributes, "Example")
    end
  end
end