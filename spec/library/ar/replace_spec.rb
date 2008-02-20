require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/archives'

require 'ar'
require 'etc'

ArSpec.new '#replace' do

  mtime = Time.at 1203462040
  uid = Etc.getpwuid.uid
  gid = Etc.getpwuid.gid
  mode = 0644

  it 'adds a file' do
    archive = write_ar ArSpec::ABC

    name = 'd'
    data = "dog\n"

    Ar.new(archive.path).replace name, mtime, uid, gid, mode, data

    Ar.new(archive.path).list.should == %w[a b c d]

    Ar.new(archive.path).extract('d').should == [
      name, mtime, uid, gid, mode, data
    ]
  end

  it 'replaces a file' do
    archive = write_ar ArSpec::ABC

    name = 'a'
    data = "albatross\n"

    Ar.new(archive.path).replace name, mtime, uid, gid, mode, data

    Ar.new(archive.path).list.should == %w[b c a]

    Ar.new(archive.path).extract('a').should == [
      name, mtime, uid, gid, mode, data
    ]
  end

  it 'raises on non-archives' do
    archive = write_ar 'not an archive'

    name = 'a'
    data = "albatross\n"

    lambda do
      Ar.new(archive.path).replace name, mtime, uid, gid, mode, data
    end.should raise_error(Ar::Error, "#{archive.path} is not an archive file")
  end

end

