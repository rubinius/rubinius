# -*- encoding: ascii-8bit -*-
require File.expand_path('../../../../spec_helper', __FILE__)
require 'stringio'
require 'zlib'

describe "GzipWriter#write" do

  before :each do
    @data = '12345abcde'
    @zip = "\037\213\b\000,\334\321G\000\00334261MLJNI\005\000\235\005\000$\n\000\000\000"
    @io = StringIO.new ""
  end

  it "writes some compressed data" do
    Zlib::GzipWriter.wrap @io do |gzio|
      gzio.write @data
    end

    # skip gzip header for now
    @io.string[10..-1].should == @zip[10..-1]
  end

end

