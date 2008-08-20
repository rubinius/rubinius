require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#form when passed no arguments" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
    
    def @html.script_name
      "/path/to/some/script"
    end
  end
  
  it "returns a 'form'-element" do
    expected = '<FORM ENCTYPE="application/x-www-form-urlencoded" METHOD="post" ACTION="/path/to/some/script"></FORM>'
    @html.form.should == expected
  end

  it "includes the return value of the passed block when passed a block" do
    expected = '<FORM ENCTYPE="application/x-www-form-urlencoded" METHOD="post" ACTION="/path/to/some/script">test</FORM>'
    @html.form { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#form when passed method" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
    
    def @html.script_name
      "/path/to/some/script"
    end
  end
  
  it "returns a 'form'-element with the passed method" do
    expected = '<FORM ENCTYPE="application/x-www-form-urlencoded" METHOD="get" ACTION="/path/to/some/script"></FORM>'
    @html.form("get").should == expected
  end

  it "includes the return value of the passed block when passed a block" do
    expected = '<FORM ENCTYPE="application/x-www-form-urlencoded" METHOD="get" ACTION="/path/to/some/script">test</FORM>'
    @html.form("get") { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#form when passed method, action" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
    
    def @html.script_name
      "/path/to/some/script"
    end
  end
  
  it "returns a 'form'-element with the passed method and the passed action" do
    expected = '<FORM ENCTYPE="application/x-www-form-urlencoded" METHOD="get" ACTION="/some/other/script"></FORM>'
    @html.form("get", "/some/other/script").should == expected
  end

  it "includes the return value of the passed block when passed a block" do
    expected = '<FORM ENCTYPE="application/x-www-form-urlencoded" METHOD="get" ACTION="/some/other/script">test</FORM>'
    @html.form("get", "/some/other/script") { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#form when passed method, action, enctype" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
    
    def @html.script_name
      "/path/to/some/script"
    end
  end
  
  it "returns a 'form'-element with the passed method, action and enctype" do
    expected = '<FORM ENCTYPE="multipart/form-data" METHOD="get" ACTION="/some/other/script"></FORM>'
    @html.form("get", "/some/other/script", "multipart/form-data").should == expected
  end
  
  it "includes the return value of the passed block when passed a block" do
    expected = '<FORM ENCTYPE="multipart/form-data" METHOD="get" ACTION="/some/other/script">test</FORM>'
    @html.form("get", "/some/other/script", "multipart/form-data") { "test" }.should == expected
  end
end