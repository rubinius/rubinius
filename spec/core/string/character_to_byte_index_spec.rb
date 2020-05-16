# encoding: utf-8
require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "String::Mirror#character_to_byte_index!" do

    before :each do
      @utf8 = Rubinius::Mirror.reflect("hello 椎名深夏" * 1000)
    end

    it "returns the byte index for the given character index" do
      @utf8.character_to_byte_index(358).should == 642
    end

    it "returns the byte index for the given character index searching from given byte index" do
      @utf8.character_to_byte_index(234, 642).should == 1064
    end

  end
end
