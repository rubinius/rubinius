require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#file_field when passed no arguments" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a file-'input'-element without a name and a size of 20" do
    @html.file_field.should == '<INPUT SIZE="20" NAME="" TYPE="file">'
  end
  
  it "ignores a passed block" do
    @html.file_field { "test" }.should == '<INPUT SIZE="20" NAME="" TYPE="file">'
  end
end

describe "CGI::HtmlExtension#file_field when passed name" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a checkbox-'input'-element with the passed name" do
    @html.file_field("test").should == '<INPUT SIZE="20" NAME="test" TYPE="file">'
  end

  it "ignores a passed block" do
    @html.file_field("test") { "test" }.should == '<INPUT SIZE="20" NAME="test" TYPE="file">'
  end
end

describe "CGI::HtmlExtension#file_field when passed name, size" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a checkbox-'input'-element with the passed name and size" do
    @html.file_field("test", 40).should == '<INPUT SIZE="40" NAME="test" TYPE="file">'
  end

  it "ignores a passed block" do
    expected = '<INPUT SIZE="40" NAME="test" TYPE="file">'
    @html.file_field("test", 40) { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#file_field when passed name, size, maxlength" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a checkbox-'input'-element with the passed name, size and maxlength" do
    expected = '<INPUT MAXLENGTH="100" SIZE="40" NAME="test" TYPE="file">'
    @html.file_field("test", 40, 100).should == expected
  end

  it "ignores a passed block" do
    expected = '<INPUT MAXLENGTH="100" SIZE="40" NAME="test" TYPE="file">'
    @html.file_field("test", 40, 100) { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#file_field when passed a Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  ruby_bug "", "1.8.7" do
    it "returns a file-'input'-element using the passed Hash for attributes" do
      expected = '<INPUT SIZE="40" NAME="test" TYPE="file">'
      @html.file_field("NAME" => "test", "SIZE" => 40).should == expected
    
      expected = '<INPUT MAXLENGTH="100" NAME="test" TYPE="file">'
      @html.file_field("NAME" => "test", "MAXLENGTH" => 100).should == expected
    end

    it "ignores a passed block" do
      expected = '<INPUT SIZE="40" NAME="test" TYPE="file">'
      @html.file_field("NAME" => "test", "SIZE" => 40) { "test" }.should == expected
    end
  end
end