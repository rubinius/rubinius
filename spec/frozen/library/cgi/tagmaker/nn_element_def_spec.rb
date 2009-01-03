require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::TagMaker#nn_element_def when passed element" do
  before(:each) do
    @obj = Object.new
    @obj.extend(CGI::TagMaker)
  end
  
  it "returns code for the passed element with required start and end tags" do
    @obj.nn_element_def("A").should == <<-EOS
          "<A" + attributes.collect{|name, value|
            next unless value
            " " + CGI::escapeHTML(name) +
            if true == value
              ""
            else
              \'="\' + CGI::escapeHTML(value) + \'"\'
            end
          }.to_s + ">" +
          if block_given?
            yield.to_s
          else
            ""
          end +
          "</A>"
EOS
  end

  it "automatically converts the element tag to capital letters" do
    @obj.nn_element_def("a").should == <<-EOS
          "<A" + attributes.collect{|name, value|
            next unless value
            " " + CGI::escapeHTML(name) +
            if true == value
              ""
            else
              \'="\' + CGI::escapeHTML(value) + \'"\'
            end
          }.to_s + ">" +
          if block_given?
            yield.to_s
          else
            ""
          end +
          "</A>"
EOS
  end
end
