require File.dirname(__FILE__) + '/../../spec_helper'
require 'getoptlong'

describe "GetoptLong#error_message" do
  it "should return nil if no error occurred" do
    opts = GetoptLong.new
    opts.error_message.should == nil
  end
  
  it "should return the error message of the last error that occurred" do
    begin
      opts = GetoptLong.new
      opts.get
      opts.ordering = GetoptLong::PERMUTE
    rescue ArgumentError
      opts.error_message.should == "argument error"
    end
  end
end