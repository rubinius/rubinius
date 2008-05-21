require File.dirname(__FILE__) + '/../../spec_helper'
require 'iconv'

describe "Iconv#close" do
  it "ignores multiple calls" do
    conv1 = Iconv.new("us-ascii", "us-ascii")
    conv1.close
    conv1.close
  end

  it "does not raise an exception if called inside an .open block" do
    Iconv.open "us-ascii", "us-ascii" do |conv2|
      conv2.close
    end
  end

  # return values of #close not tested yet
end
