require File.dirname(__FILE__) + '/../../../spec_helper'
require 'stringio'
require 'zlib'

describe "GzipReader#rewind" do

  before :each do
    @data = '12345abcde'
    @zip = "\037\213\b\000,\334\321G\000\00334261MLJNI\005\000\235\005\000$\n\000\000\000"
    @io = StringIO.new @zip
  end

  it "resets the position of the file pointer" do
    gz = Zlib::GzipReader.new @io
    gz.read
    gz.pos.should == @data.length

    gz.rewind
    gz.pos.should == 0
  end

  it "invokes seek method on the associated IO object" do
    # first, prepare the mock object:
    (obj = mock("io")).should_receive(:get_io).any_number_of_times.and_return(@io)
    def obj.read(args); get_io.read(args); end
    obj.should_receive(:seek).and_return(0)

    gz = Zlib::GzipReader.new(obj)
    gz.rewind()
    gz.pos.should == 0
  end
end
