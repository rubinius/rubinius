require File.dirname(__FILE__) + '/../../spec_helper'
require 'iconv'

describe "Iconv.charset_map" do
  it "acts as a map" do
    Iconv.charset_map.respond_to?(:[]).should be_true
  end
end
