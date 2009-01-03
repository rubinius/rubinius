require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'

describe "CGI::TagMaker#nO_element_def when passed element" do
  before(:each) do
    @obj = Object.new
    @obj.extend(CGI::TagMaker)
  end

  it "returns code for the passed element with optional start/end tags" do
    @obj.nO_element_def("P").should == <<-EOS
          "<P" + attributes.collect{|name, value|
            next unless value
            " " + CGI::escapeHTML(name) +
            if true == value
              ""
            else
              \'="\' + CGI::escapeHTML(value) + \'"\'
            end
          }.to_s + ">" +
          if block_given?
            yield.to_s + "</P>"
          else
            ""
          end
EOS
  end

  it "automatically converts the element tag to capital letters" do
    @obj.nO_element_def("p").should == <<-EOS
          "<P" + attributes.collect{|name, value|
            next unless value
            " " + CGI::escapeHTML(name) +
            if true == value
              ""
            else
              \'="\' + CGI::escapeHTML(value) + \'"\'
            end
          }.to_s + ">" +
          if block_given?
            yield.to_s + "</P>"
          else
            ""
          end
EOS
  end

end
