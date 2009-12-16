require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do
  
  require 'ping'

  describe "Ping.pingecho" do

    it "responds to pingecho method" do
      Ping.should respond_to(:pingecho)
    end

    it "pings a host using the correct number of arguments" do
      Ping.pingecho('127.0.0.1', 10, 7).should be_true
      Ping.pingecho('127.0.0.1', 10).should be_true
      Ping.pingecho('127.0.0.1').should be_true
    end

    it "raises ArgumentError for wrong number of arguments" do
      lambda { Ping.pingecho }.should raise_error(ArgumentError)
      lambda { Ping.pingecho 'one', 'two', 'three', 'four' }.should raise_error(ArgumentError)
    end

    it "returns false for invalid parameters" do
      Ping.pingecho('127.0.0.1', 5, 'invalid port').should be_false
      Ping.pingecho('127.0.0.1', 'invalid timeout').should be_false
      Ping.pingecho(123).should be_false
    end

  end
end
