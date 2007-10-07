require File.dirname(__FILE__) + '/../../spec_helper'
require 'getoptlong'

describe "GetoptLong#initialize" do
  it "should set ordering to REQUIRE_ORDER if ENV['POSIXLY_CORRECT'] is set" do
    begin
      old_env_value = ENV["POSIXLY_CORRECT"]
      ENV["POSIXLY_CORRECT"] = ""
      
      opt = GetoptLong.new
      opt.ordering.should == GetoptLong::REQUIRE_ORDER
    ensure
      ENV["POSIXLY_CORRECT"] = old_env_value
    end
  end
  
  it "should set ordering to PERMUTE if ENV['POSIXLY_CORRECT'] is not set" do
    begin
      old_env_value = ENV["POSIXLY_CORRECT"]
      ENV["POSIXLY_CORRECT"] = nil
      
      opt = GetoptLong.new
      opt.ordering.should == GetoptLong::PERMUTE
    ensure
      ENV["POSIXLY_CORRECT"] = old_env_value
    end
  end
end