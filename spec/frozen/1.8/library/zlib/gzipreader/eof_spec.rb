require File.dirname(__FILE__) + '/../../../spec_helper'
require 'stringio'
require 'zlib'

describe "GzipReader#eof?" do

  before :each do
    @data = '12345abcde'
    @zip = "\037\213\b\000,\334\321G\000\00334261MLJNI\005\000\235\005\000$\n\000\000\000"
    @io = StringIO.new @zip
  end

  it "returns true when at EOF" do
    gz = Zlib::GzipReader.new @io

    gz.eof?.should == false
    gz.read
    gz.eof?.should == true
  end

end

