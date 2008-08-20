require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/http'

describe "Net::HTTP#edit_path" do
  it "is private" do
    Net::HTTP.private_instance_methods.should include("edit_path")
  end
  
  it "returns the passed path" do
    net = Net::HTTP.new("localhost", 3333)
    net.send(:edit_path, "/test.html").should == "/test.html"
  end
end
