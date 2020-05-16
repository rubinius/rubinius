# encoding: utf-8
require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "String::Mirror#character_to_byte_index!" do

    before :each do
      @utf8 = Rubinius::Mirror.reflect("hello 椎名深夏" * 1000)
    end

    it "returnse character index for the character containing the byte at the given byte index" do
      @utf8.byte_to_character_index(358).should == 200
    end

    it "returns the character index offset for the character containing the byte at the given byte index searching from given byte index" do
      @utf8.byte_to_character_index(234, 642).should == 130
    end

  end
end

