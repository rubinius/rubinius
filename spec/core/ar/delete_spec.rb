require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe 'Ar#delete' do
  after :each do
    @archive.close if @archive
  end
  
  it 'deletes many files' do
    @archive = ArSpec.write_ar ArSpec::ABC

    Ar.new(@archive.path).delete 'a', 'b'

    Ar.new(@archive.path).list.should == %w[c]
  end

  it 'deletes one file' do
    @archive = ArSpec.write_ar ArSpec::ABC

    Ar.new(@archive.path).delete 'a'

    Ar.new(@archive.path).list.should == %w[b c]
  end

  it 'handles long names' do
    @archive = ArSpec.write_ar ArSpec::LONG_NAME

    Ar.new(@archive.path).delete 'really_long_file_name'

    Ar.new(@archive.path).list.should == %w[
      12345678901234567 12345678901234567890
    ]
  end
end
