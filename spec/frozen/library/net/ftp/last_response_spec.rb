require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require File.dirname(__FILE__) + "/fixtures/server"

describe "Net::FTP#last_response" do
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

  it "returns the last response" do
    @ftp.last_response.should == "220 Dummy FTP Server ready!\n"
    @ftp.help
    @ftp.last_response.should == "211 System status, or system help reply. (HELP)\n"
  end
end
