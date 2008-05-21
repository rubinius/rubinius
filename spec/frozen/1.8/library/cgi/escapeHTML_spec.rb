require File.dirname(__FILE__) + '/../../spec_helper'
require 'cgi'

describe "CGI.escapeHTML" do

  it "escape '& < > \"' to '&amp; &lt; &gt; &quot;" do
    input = '& < > "'
    expected = '&amp; &lt; &gt; &quot;'
    CGI::escapeHTML(input).should == expected
  end

  it "not escape characters except '& < > \"'" do
    input = (0x20..0x7E).to_a.collect {|ch| ch.chr}.join('')
    expected = input.gsub(/&/,'&amp;').gsub(/</,'&lt;').gsub(/>/,'&gt;').gsub(/"/,'&quot;')
    CGI::escapeHTML(input).should == expected
  end

  it "raise error when argument is not a string" do
    lambda { CGI::escapeHTML(nil)  }.should raise_error(Exception) # NoMethodError
    lambda { CGI::escapeHTML(123)  }.should raise_error(Exception) # NoMethodError
    lambda { CGI::escapeHTML(true) }.should raise_error(Exception) # NoMethodError
  end

end
