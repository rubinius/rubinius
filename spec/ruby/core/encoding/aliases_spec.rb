require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Encoding.aliases" do
    it "returns a Hash" do
      Encoding.aliases.should be_an_instance_of(Hash)
    end

    it "has Strings as keys" do
      Encoding.aliases.keys.each do |key|
        key.should be_an_instance_of(String)
      end
    end

    it "has Strings as values" do
      Encoding.aliases.values.each do |value|
        value.should be_an_instance_of(String)
      end
    end

    it "has alias names as its keys" do
      Encoding.aliases.key?('BINARY').should be_true
      Encoding.aliases.key?('ASCII').should be_true
    end

    it "has the names of the aliased encoding as its values" do
      Encoding.aliases['BINARY'].should == 'ASCII-8BIT'
      Encoding.aliases['ASCII'].should == 'US-ASCII'
    end

    it "has an 'external' key with the external default encoding as its value" do
      Encoding.aliases['external'].should == Encoding.default_external.name
    end

    it "has a 'locale' key with the locale charmap encoding as its value" do
      Encoding.aliases['locale'].should == Encoding.locale_charmap
    end

    it "only contains valid aliased encodings" do
      Encoding.aliases.each do |enc_alias|
        lambda { Encoding.find(enc_alias) }.should_not raise_error(ArgumentError)
      end
    end
  end
end