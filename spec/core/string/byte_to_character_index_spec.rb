# encoding: utf-8
require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "String::Mirror#character_to_byte_index!" do

    before :each do
      @utf8 = Rubinius::Mirror.reflect("hello 椎名深夏" * 1000)
      @utf32 = Rubinius::Mirror.reflect(@utf8.object.encode(Encoding::UTF_32BE))
      @shift_jis = Rubinius::Mirror.reflect(@utf8.object.encode(Encoding::SHIFT_JIS))
    end

    it "returnse character index for the character containing the byte at the given byte index" do
      @utf8.byte_to_character_index(358).should == 200
      @utf32.byte_to_character_index(358).should == 89
      @shift_jis.byte_to_character_index(358).should == 257
    end

    it "returns the character index offset for the character containing the byte at the given byte index searching from given byte index" do
      @utf8.byte_to_character_index(234, 642).should == 130
      @utf32.byte_to_character_index(234, 1432).should == 58
      @shift_jis.byte_to_character_index(234, 500).should == 168
    end

  end
end

