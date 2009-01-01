require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"

describe "Net::FTP#welcome" do
  before(:each) do
    @server = NetFTPSpecs::DummyFTP.new
    @server.serve_once

    @ftp = Net::FTP.new
    @ftp.connect("localhost", 9921)
  end

  after(:each) do
    @ftp.quit rescue nil
    @ftp.close
    @server.stop
  end
  
  it "returns the server's welcome message" do
    @ftp.welcome.should be_nil
    @ftp.login
    @ftp.welcome.should == "230 User logged in, proceed. (USER anonymous)\n"
  end
end
