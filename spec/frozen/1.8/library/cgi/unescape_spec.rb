require File.dirname(__FILE__) + '/../../spec_helper'
require 'cgi'

describe "CGI.unescape" do

  it "decode '%' format string" do
    input = '%a4%Aa'
    expected = "\244\252"
    CGI::unescape(input).should == expected
  end

  it "leave invalid format string as is" do
    input = "%XX%5%%%"
    expected = input
    CGI::unescape(input).should == expected
  end

  it "raise error when non-string is passed" do
    lambda { CGI::unescape(nil)  }.should raise_error(Exception)  # NoMethodError
    lambda { CGI::unescape(123)  }.should raise_error(Exception)  # NoMethodError
    lambda { CGI::unescape(true) }.should raise_error(Exception)  # NoMethodError
  end

end
