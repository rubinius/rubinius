require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe 'Ar#write' do
  after :each do
    @archive.close if @archive
  end

  it 'writes a new ar file' do
    @archive = ArSpec.write_ar ArSpec::ABC

    Tempfile.open 'ABC.a' do |io|
      Ar.new(@archive.path).write io

      io.rewind

      io.read.should == ArSpec::ABC
    end
  end

  it 'handles long names' do
    @archive = ArSpec.write_ar ArSpec::LONG_NAME

    Tempfile.open 'ABC.a' do |io|
      Ar.new(@archive.path).write io

      io.rewind

      io.read.should == ArSpec::LONG_NAME
    end
  end
end
