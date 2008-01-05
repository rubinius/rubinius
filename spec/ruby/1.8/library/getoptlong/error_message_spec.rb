require File.dirname(__FILE__) + '/../../spec_helper'
require 'getoptlong'

describe "GetoptLong#error_message" do
  it "returns nil if no error occurred" do
    opts = GetoptLong.new
    opts.error_message.should == nil
  end
  
  it "returns the error message of the last error that occurred" do
    begin
      s = $stderr
      $stderr = CaptureOutput.new
      opts = GetoptLong.new
      opts.get
      opts.ordering = GetoptLong::PERMUTE
    rescue ArgumentError
      opts.error_message.should == "argument error"
    ensure
      $stderr = s
    end
  end
end