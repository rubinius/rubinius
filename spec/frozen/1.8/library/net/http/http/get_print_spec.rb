require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + '/fixtures/http_server'

describe "Net::HTTP.get_print" do
  before(:all) do
    NetHTTPSpecs.start_server
  end
  
  after(:all) do
    NetHTTPSpecs.stop_server
  end

  describe "when passed URI" do
    it "it prints the body of the specified uri to $stdout" do
      $stdout.should_receive(:print).with("")
      $stdout.should_receive(:print).with("This is the index page.")
      Net::HTTP.get_print URI.parse('http://localhost:3333/')
    end
  end

  describe "when passed host, path, port" do
    it "it prints the body of the specified uri to $stdout" do
      $stdout.should_receive(:print).with("")
      $stdout.should_receive(:print).with("This is the index page.")
      Net::HTTP.get_print 'localhost', "/", 3333
    end
  end
end
