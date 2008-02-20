require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/archives'

require 'ar'

ArSpec.new '#extract' do

  mtime = Time.at(1203110500).utc
  uid = 501
  gid = 20
  mode = 0100644

  it 'returns a file' do
    archive = write_ar ArSpec::ABC

    Ar.new(archive.path).extract('a').should == [
      'a', mtime, uid, gid, mode, "apple\n"
    ]

    Ar.new(archive.path).extract('b').should == [
      'b', mtime, uid, gid, mode, "balloon\n"
    ]

    Ar.new(archive.path).extract('c').should == [
      'c', mtime, uid, gid, mode, "crayon\n"
    ]
  end

  it 'returns a file with a long name' do
    archive = write_ar ArSpec::LONG_NAME

    Ar.new(archive.path).extract('12345678901234567').should == [
      '12345678901234567', mtime, uid, gid, mode, 
      "one two three ... seventeen\n"
    ]

    Ar.new(archive.path).extract('12345678901234567890').should == [
      '12345678901234567890', mtime, uid, gid, mode,
      "one two three ... twenty\n"
    ]
  end

end

