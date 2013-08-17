# encoding: utf-8
require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "String::Mirror#character_to_byte_index!" do

    before :each do
      @utf8 = Rubinius::Mirror.reflect("hello 椎名深夏" * 1000)
      @utf32 = Rubinius::Mirror.reflect(@utf8.object.encode(Encoding::UTF_32BE))
      @shift_jis = Rubinius::Mirror.reflect(@utf8.object.encode(Encoding::SHIFT_JIS))
    end

    it "returns the byte index for the given character index" do
      @utf8.character_to_byte_index(358).should == 642
      @utf32.character_to_byte_index(358).should == 1432
      @shift_jis.character_to_byte_index(358).should == 500
    end

    it "returns the byte index for the given character index searching from given byte index" do
      @utf8.character_to_byte_index(234, 642).should == 1064
      @utf32.character_to_byte_index(234, 1432).should == 2368
      @shift_jis.character_to_byte_index(234, 500).should == 828
    end

  end
end
