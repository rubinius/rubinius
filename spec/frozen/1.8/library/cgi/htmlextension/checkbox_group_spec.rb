require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::HtmlExtension#checkbox_group when passed name, values ..." do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "returns a sequence of 'checkbox'-elements with the passed name and the passed values" do
    expected = '<INPUT NAME="test" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="baz">baz'
    @html.checkbox_group("test", "foo", "bar", "baz").should == expected
  end
  
  it "allows passing a value inside an Array" do
    expected = '<INPUT NAME="test" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="baz">baz'
    @html.checkbox_group("test", ["foo"], "bar", ["baz"]).should == expected
  end
  
  it "allows passing a value as an Array containing the value and the checked state or a label" do
    expected = '<INPUT NAME="test" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT CHECKED NAME="test" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="baz">label for baz'
    @html.checkbox_group("test", ["foo"], ["bar", true], ["baz", "label for baz"]).should == expected
  end
  
  # TODO: CGI does not like passing false instead of true.
  ruby_bug "", "1.8.7" do
    it "allows passing a value as an Array containing the value, a label and the checked state" do
      expected = '<INPUT CHECKED NAME="test" TYPE="checkbox" VALUE="foo">label for foo' +
        '<INPUT NAME="test" TYPE="checkbox" VALUE="bar">label for bar' +
        '<INPUT CHECKED NAME="test" TYPE="checkbox" VALUE="baz">label for baz'
      @html.checkbox_group("test", ["foo", "label for foo", true], ["bar", "label for bar", false], ["baz", "label for baz", true]).should == expected
    end
  end
  
  it "returns an empty String when passed no values" do
    @html.checkbox_group("test").should == ""
  end
  
  it "ignores a passed block" do
    expected = '<INPUT NAME="test" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="test" TYPE="checkbox" VALUE="baz">baz'
    @html.checkbox_group("test", "foo", "bar", "baz") { "test" }.should == expected
  end
end

describe "CGI::HtmlExtension#checkbox_group when passed Hash" do
  before(:each) do
    @html = Object.new
    @html.extend(CGI::Html4)
    @html.element_init
    @html.extend(CGI::HtmlExtension)
  end

  it "uses the passed Hash to generate the checkbox sequence" do
    expected = '<INPUT NAME="name" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT NAME="name" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="name" TYPE="checkbox" VALUE="baz">baz'
    @html.checkbox_group("NAME" => "name", "VALUES" => ["foo", "bar", "baz"]).should == expected

    expected = '<INPUT NAME="name" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT CHECKED NAME="name" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="name" TYPE="checkbox" VALUE="baz">baz'
    @html.checkbox_group("NAME" => "name", "VALUES" => [["foo"], ["bar", true], "baz"]).should == expected

    expected = '<INPUT NAME="name" TYPE="checkbox" VALUE="1">Foo' +
      '<INPUT CHECKED NAME="name" TYPE="checkbox" VALUE="2">Bar' +
      '<INPUT NAME="name" TYPE="checkbox" VALUE="Baz">Baz'
    @html.checkbox_group("NAME" => "name", "VALUES" => [["1", "Foo"], ["2", "Bar", true], "Baz"]).should == expected
  end

  it "ignores a passed block" do
    expected = '<INPUT NAME="name" TYPE="checkbox" VALUE="foo">foo' +
      '<INPUT NAME="name" TYPE="checkbox" VALUE="bar">bar' +
      '<INPUT NAME="name" TYPE="checkbox" VALUE="baz">baz'
    @html.checkbox_group("NAME" => "name", "VALUES" => ["foo", "bar", "baz"]) { "test" }.should == expected
  end
end