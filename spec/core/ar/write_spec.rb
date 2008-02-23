require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/archives'

require 'ar'

ArSpec.new '#write' do

  it 'writes a new ar file' do
    archive = write_ar ArSpec::ABC

    Tempfile.open 'ABC.a' do |io|
      Ar.new(archive.path).write io

      io.rewind

      io.read.should == ArSpec::ABC
    end
  end

  it 'handles long names' do
    archive = write_ar ArSpec::LONG_NAME

    Tempfile.open 'ABC.a' do |io|
      Ar.new(archive.path).write io

      io.rewind

      io.read.should == ArSpec::LONG_NAME
    end

  end

end

