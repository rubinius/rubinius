require File.dirname(__FILE__) + '/../../../spec_helper'

ruby_version_is "1.9" do
  describe "Encoding::Converter#source_encoding" do
    it "returns the source encoding as an Encoding object" do
      ec = Encoding::Converter.new('ASCII','Big5')
      ec.source_encoding.should == Encoding::US_ASCII

      ec = Encoding::Converter.new('SJIS','EUC-JP')
      ec.source_encoding.should == Encoding::SHIFT_JIS
    end
  end
end
