require File.dirname(__FILE__) + '/../../../spec_helper'
require 'iconv'

describe "Iconv::Failure#failed" do
  it "returns a substring of the original string passed to Iconv that starts at the character which caused the exception" do
    lambda {
      begin
        Iconv.open "us-ascii", "us-ascii" do |conv|
          conv.iconv "test \xff test \xff"
        end
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.failed.should == "\xff test \xff"

    lambda {
      begin
        Iconv.open "utf-8", "utf-8" do |conv|
          conv.iconv "test \xe2\x82"
        end
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.failed.should == "\xe2\x82"
  end

  it "for Iconv.iconv and Iconv.conv returns an array containing a single element when instantiated" do
    lambda {
      begin
        Iconv.iconv("us-ascii", "us-ascii", "test \xff test")
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.failed.should == ["\xff test"]

    lambda {
      begin
        Iconv.conv("us-ascii", "us-ascii", "test \xff test")
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.failed.should == ["\xff test"]
  end
end
