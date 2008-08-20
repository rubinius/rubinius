require File.dirname(__FILE__) + '/../../../spec_helper'
require 'iconv'

describe "Iconv::Failure#success" do
  it "for Iconv#iconv and Iconv.conv returns the substring of the original string passed which was translated successfully until the exception ocurred" do
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
    @ex.success.should == "test "

    lambda {
      begin
        Iconv.conv "utf-8", "utf-8", "\xe2\x82"
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.success.should == ""
  end

  it "for Iconv.iconv returns an array containing all the strings that were translated successfully until the exception ocurred, in order" do
    lambda {
      begin
        Iconv.iconv("us-ascii", "us-ascii", "\xfferror")
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.success.should == [""]

    lambda {
      begin
        Iconv.iconv("us-ascii", "us-ascii", "test", "testing", "until\xfferror")
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    @ex.success.should == ["test", "testing", "until"]
  end
end
