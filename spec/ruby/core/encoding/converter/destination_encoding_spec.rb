require File.dirname(__FILE__) + '/../../../spec_helper'

ruby_version_is "1.9" do
  describe "Encoding::Converter#destination_encoding" do
    it "returns the destination encoding as an Encoding object" do
      ec = Encoding::Converter.new('ASCII','Big5')
      ec.destination_encoding.should == Encoding::BIG5

      ec = Encoding::Converter.new('SJIS','EUC-JP')
      ec.destination_encoding.should == Encoding::EUC_JP
    end
  end
end
