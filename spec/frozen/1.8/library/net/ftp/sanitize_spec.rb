require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#sanitize" do
  before(:each) do
    @ftp = Net::FTP.new
  end
  
  it "sanitizes the given string by replacing passwords with stars" do
    @ftp.send(:sanitize, "PASS my password here").should == "PASS ****************"
    @ftp.send(:sanitize, "pass my password here").should == "pass ****************"

    @ftp.send(:sanitize, "Nothing Special").should == "Nothing Special"
    @ftp.send(:sanitize, "PAS not sanitized").should == "PAS not sanitized"
    @ftp.send(:sanitize, "PASSnot sanitized").should == "PASSnot sanitized"
  end
end
