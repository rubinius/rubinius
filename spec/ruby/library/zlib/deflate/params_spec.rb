require File.dirname(__FILE__) + '/../../../spec_helper'
require 'zlib'

describe 'Zlib::Deflate#params' do
  ruby_bug '239', '1.9.0' do
  it 'changes the deflate parameters' do
    data = 'abcdefghijklm'

    d = Zlib::Deflate.new Zlib::NO_COMPRESSION, Zlib::MAX_WBITS,
    Zlib::DEF_MEM_LEVEL, Zlib::DEFAULT_STRATEGY

    d << data.slice!(0..10)
    d.params Zlib::BEST_COMPRESSION, Zlib::DEFAULT_STRATEGY
    d << data

    d.finish.should ==
      "x\001\000\v\000\364\377abcdefghijk\002,'\027\000\#\364\005<"
  end
  end
end

