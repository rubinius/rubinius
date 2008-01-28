require File.dirname(__FILE__) + '/../../../spec_helper'

describe 'File::Stat#dev' do

  it 'returns the device number' do
    fname = 'test_file'
    File.open fname, 'w' do |fp| fp.puts fname end
    stat = File.stat fname

    stat.dev.should > 0
  end

end

