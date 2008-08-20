require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'

describe "Net::HTTP#conn_address" do
  it "is private" do
    Net::HTTP.private_instance_methods.should include("conn_address")
  end
  
  it "returns the current host name" do
    net = Net::HTTP.new("localhost")
    net.send(:conn_address).should == "localhost"
  end
end
