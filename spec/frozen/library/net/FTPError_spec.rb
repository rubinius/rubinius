require File.dirname(__FILE__) + '/../../spec_helper'
require 'net/ftp'

describe "Net::FTPError" do
  it "is an Exception" do
    Net::FTPError.should < Exception
  end
end
