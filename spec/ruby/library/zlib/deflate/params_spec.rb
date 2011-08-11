require File.expand_path('../../../../spec_helper', __FILE__)
require 'zlib'

describe 'Zlib::Deflate#params' do
  # This spec appears to be dependent on the version of zlib.
  # It appears that version 1.2.3+ will cause this spec to fail.
  # If that's true, we need some way to specify zlib version guards
  # on top of rubyspec version guards. Which all sounds like an
  # incredibly big pile of fun.
  quarantine! do
  ruby_bug '239', '1.9.0' do
  it 'changes the deflate parameters' do
    data = 'abcdefghijklm'

    d = Zlib::Deflate.new Zlib::NO_COMPRESSION, Zlib::MAX_WBITS,
    Zlib::DEF_MEM_LEVEL, Zlib::DEFAULT_STRATEGY

    d << data.slice!(0..10)
    d.params Zlib::BEST_COMPRESSION, Zlib::DEFAULT_STRATEGY
    d << data

    Zlib::Inflate.inflate(d.finish).should == 'abcdefghijklm'
  end
  end
  end
end

