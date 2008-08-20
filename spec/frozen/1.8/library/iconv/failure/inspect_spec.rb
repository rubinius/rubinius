require File.dirname(__FILE__) + '/../../../spec_helper'
require 'iconv'

describe "Iconv::Failure#inspect" do
  it "includes information on the exception class name, #succes and #failed" do
    lambda {
      begin
        Iconv.open "utf-8", "utf-8" do |conv|
          conv.iconv "testing string \x80 until an error occurred"
        end
      rescue Iconv::Failure => e
        @ex = e
        raise e
      end
    }.should raise_error(Iconv::Failure)
    inspection = @ex.inspect
    inspection.should include(@ex.class.to_s)
    inspection.should include(@ex.success.inspect)
    inspection.should include(@ex.failed.inspect)
  end
end
