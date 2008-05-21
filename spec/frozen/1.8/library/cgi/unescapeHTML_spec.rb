require File.dirname(__FILE__) + '/../../spec_helper'
require 'cgi'

describe "CGI.unescapeHTML" do

  it "unescape '&amp; &lt; &gt; &quot;' to '& < > \"'" do
    input = '&amp; &lt; &gt; &quot;'
    expected = '& < > "'
    CGI::unescapeHTML(input).should == expected
  end

  it "doesn't unescape other html entities such as '&copy;' or '&heart'" do
    input = '&copy;&heart;'
    expected = input
    CGI::unescapeHTML(input).should == expected
  end

  it "unescape '&#99' format entities" do
    input = '&#34;&#38;&#39;&#60;&#62;'
    expected = '"&\'<>'
    CGI::unescapeHTML(input).should == expected
  end

  it "unescape '&#x9999' format entities" do
    input = '&#x0022;&#x0026;&#x0027;&#x003c;&#x003E;'
    expected = '"&\'<>'
    CGI::unescapeHTML(input).should == expected
  end

  it "leave invalid format string" do
    input = '&&lt;&amp&gt;&quot&abcdefghijklmn'
    expected = '&<&amp>&quot&abcdefghijklmn'
    CGI::unescapeHTML(input).should == expected
  end

  it "raise error when non-string is passed" do
    lambda { CGI::unescapeHTML(nil)  }.should raise_error(Exception)
    lambda { CGI::unescapeHTML(123)  }.should raise_error(Exception)
    lambda { CGI::unescapeHTML(true) }.should raise_error(Exception)
  end

end
