require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/archives'

require 'ar'

ArSpec.new '#list' do

  it 'handles long names' do
    archive = write_ar ArSpec::LONG_NAME

    Ar.new(archive.path).list.should == %w[
      really_long_file_name 12345678901234567 12345678901234567890]
  end

  it 'returns a list of files' do
    archive = write_ar ArSpec::ABC

    Ar.new(archive.path).list.should == %w[a b c]
  end

end

