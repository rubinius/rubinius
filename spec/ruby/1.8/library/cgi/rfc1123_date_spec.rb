require File.dirname(__FILE__) + '/../../spec_helper'
require 'cgi'

describe "CGI.rfc1123_date" do

  it "convert Time object into RFC1123 format such as 'Sat, 01 Dec 2007 15:56:42 GMT'" do
    sec = 1196524602
    input = Time.at(sec)
    expected = 'Sat, 01 Dec 2007 15:56:42 GMT'
    CGI.rfc1123_date(input).should == expected
  end

  it "raise error when argument is not Time object" do
    str = 'Sat, 01 Dec 2007 15:56:42 GMT'
    lambda { CGI.rfc1123_date(str) }.should raise_error(Exception)  # NoMethodError
  end

end
