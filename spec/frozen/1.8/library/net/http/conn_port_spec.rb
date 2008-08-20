require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'

describe "Net::HTTP#conn_port" do
  it "is private" do
    Net::HTTP.private_instance_methods.should include("conn_port")
  end
  
  it "returns the current port" do
    net = Net::HTTP.new("localhost")
    net.send(:conn_port).should eql(80)

    net = Net::HTTP.new("localhost", 3333)
    net.send(:conn_port).should eql(3333)
  end
end
