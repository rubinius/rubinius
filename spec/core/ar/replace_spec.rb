require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require 'etc'

describe 'Ar#replace' do
  before :all do
    @mtime = Time.at 1203462040
    @uid = Etc.getpwuid.uid
    @gid = Etc.getpwuid.gid
    @mode = 0644
  end

  after :each do
    @archive.close if @archive
  end
  
  it 'adds a file' do
    @archive = ArSpec.write_ar ArSpec::ABC

    name = 'd'
    data = "dog\n"

    Ar.new(@archive.path).replace name, @mtime, @uid, @gid, @mode, data

    Ar.new(@archive.path).list.should == %w[a b c d]

    Ar.new(@archive.path).extract('d').should == [
      name, @mtime, @uid, @gid, @mode, data
    ]
  end

  it 'creates an @archive' do
    name = 'a'
    data = "apple\n"

    Tempfile.open 'nonexistent_ar' do |io|
      File.delete io.path

      Ar.new(io.path).replace name, @mtime, @uid, @gid, @mode, data

      Ar.new(io.path).extract('a').should == [
        name, @mtime, @uid, @gid, @mode, data
      ]
    end
  end

  it 'raises on non-archives' do
    @archive = ArSpec.write_ar 'not an archive'

    name = 'a'
    data = "albatross\n"

    lambda do
      Ar.new(@archive.path).replace name, @mtime, @uid, @gid, @mode, data
    end.should raise_error(Ar::Error, "#{@archive.path} is not an archive file")
  end

  it 'replaces a file' do
    @archive = ArSpec.write_ar ArSpec::ABC

    name = 'a'
    data = "albatross\n"

    Ar.new(@archive.path).replace name, @mtime, @uid, @gid, @mode, data

    Ar.new(@archive.path).list.should == %w[b c a]

    Ar.new(@archive.path).extract('a').should == [
      name, @mtime, @uid, @gid, @mode, data
    ]
  end
end
