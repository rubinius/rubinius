require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#hidden when passed no arguments" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns an hidden-'input'-element without a name" do
    @html.hidden.should == "<INPUT NAME=\"\" TYPE=\"hidden\">"
  end
    
  it "ignores a passed block" do
    @html.hidden { "test" }.should == "<INPUT NAME=\"\" TYPE=\"hidden\">"
  end
end

describe "CGI::HtmlExtension#hidden when passed name" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns an hidden-'input'-element with the passed name" do
    @html.hidden("test").should == "<INPUT NAME=\"test\" TYPE=\"hidden\">"
  end
    
  it "ignores a passed block" do
    @html.hidden("test") { "test" }.should == "<INPUT NAME=\"test\" TYPE=\"hidden\">"
  end
end

describe "CGI::HtmlExtension#hidden when passed name, value" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns an hidden-'input'-element with the passed name and value" do
    expected = '<INPUT NAME="test" TYPE="hidden" VALUE="some value">'
    @html.hidden("test", "some value").should == expected
  end
    
  it "ignores a passed block" do
    expected = '<INPUT NAME="test" TYPE="hidden" VALUE="some value">'
    @html.hidden("test", "some value") { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#hidden when passed Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a checkbox-'input'-element using the passed Hash for attributes" do
    expected = '<INPUT NAME="test" TYPE="hidden" VALUE="test-value">'
    @html.hidden("NAME" => "test", "VALUE" => "test-value").should == expected
  end
  
  it "ignores a passed block" do
    expected = '<INPUT NAME="test" TYPE="hidden" VALUE="test-value">'
    @html.hidden("NAME" => "test", "VALUE" => "test-value") { "test" }.should == expected
  end
end