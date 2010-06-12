require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe 'Ar#list' do
  after :each do
    @archive.close if @archive
  end
  
  it 'handles long names' do
    @archive = ArSpec.write_ar ArSpec::LONG_NAME

    Ar.new(@archive.path).list.should == %w[
      really_long_file_name 12345678901234567 12345678901234567890]
  end

  it 'returns a list of files' do
    @archive = ArSpec.write_ar ArSpec::ABC

    Ar.new(@archive.path).list.should == %w[a b c]
  end
end
