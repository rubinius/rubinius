require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#checkbox when passed no arguments" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a checkbox-'input'-element without a name" do
    @html.checkbox.should == "<INPUT NAME=\"\" TYPE=\"checkbox\">"
  end
    
  it "ignores a passed block" do
    @html.checkbox { "test" }.should == "<INPUT NAME=\"\" TYPE=\"checkbox\">"
  end
end
  
describe "CGI::HtmlExtension#checkbox when passed name" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a checkbox-'input'-element with the passed name" do
    @html.checkbox("test").should == "<INPUT NAME=\"test\" TYPE=\"checkbox\">"
  end
  
  it "ignores a passed block" do
    @html.checkbox("test") { "test" }.should == "<INPUT NAME=\"test\" TYPE=\"checkbox\">"
  end
end

describe "CGI::HtmlExtension#checkbox when passed name, value" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a checkbox-'input'-element with the passed name and value" do
    expected = "<INPUT NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("test", "test-value").should == expected
  end
  
  it "ignores a passed block" do
    expected = "<INPUT NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("test", "test-value") { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#checkbox when passed name, value, checked" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a checked checkbox-'input'-element with the passed name and value when checked is true" do
    expected = "<INPUT CHECKED NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("test", "test-value", true).should == expected
    
    expected = "<INPUT NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("test", "test-value", false).should == expected
    
    expected = "<INPUT NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("test", "test-value", nil).should == expected
  end
  
  it "ignores a passed block" do
    expected = "<INPUT NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("test", "test-value", nil) { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#checkbox when passed Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end
  
  it "returns a checkbox-'input'-element using the passed Hash for attributes" do
    expected = "<INPUT CHECKED NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("NAME" => "test", "VALUE" => "test-value", "CHECKED" => true).should == expected
  end
  
  it "ignores a passed block" do
    expected = "<INPUT CHECKED NAME=\"test\" TYPE=\"checkbox\" VALUE=\"test-value\">"
    @html.checkbox("NAME" => "test", "VALUE" => "test-value", "CHECKED" => true) { "test" }.should == expected
  end
end
