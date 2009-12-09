# coding: utf-8
require File.dirname(__FILE__) + '/shared/encode'

ruby_version_is "1.9" do
  describe "String#encode with no arguments" do
    before(:each) do
      @original_encoding = Encoding.default_internal
    end

    after(:each) do
      Encoding.default_internal = @original_encoding
    end

    it "returns a copy of self" do
      str = "strung"
      copy = str.encode
      copy.object_id.should_not == str.object_id

      # make sure that there is no sharing on byte level
      copy[0] = 'X'
      str.should == "strung"
    end

    it "returns a copy of self transcoded to Encoding.default_internal" do
      Encoding.default_internal = Encoding::UTF_8
      str = "strung"
      copy = str.encode
      copy.object_id.should_not == str.object_id

      # make sure that there is no sharing on byte level
      copy[0] = 'X'
      str.should == "strung"

      copy.encoding.should == Encoding::UTF_8
    end
  end

  describe "String#encode" do
    it_behaves_like :encode_string, :encode

    it "returns a copy of self when called with only a target encoding" do
      str = "strung".force_encoding(Encoding::UTF_8)
      copy = str.encode('ascii')
      str.encoding.should == Encoding::UTF_8
      copy.encoding.should == Encoding::US_ASCII

      str = "caf\xe9".force_encoding("iso-8859-1")
      copy = str.encode("utf-8")
      copy.encoding.should == Encoding::UTF_8
      copy.should == "caf\u00E9".force_encoding(Encoding::UTF_8)
    end

    it "returns self when called with only a target encoding" do
      str = "strung"
      copy = str.encode(Encoding::BINARY,Encoding::ASCII)
      copy.object_id.should_not == str.object_id
      str.encoding.should == Encoding::UTF_8
    end
    
    it "returns a copy of self even when no changes are made" do
      str = "strung".force_encoding('ASCII')
      str.encode(Encoding::UTF_8).object_id.should_not == str.object_id
      str.encoding.should == Encoding::US_ASCII
    end
    
    it "returns a String with the given encoding" do
      str = "ürst"
      str.encoding.should == Encoding::UTF_8
      copy = str.encode(Encoding::UTF_16LE)
      copy.encoding.should == Encoding::UTF_16LE
      str.encoding.should == Encoding::UTF_8
    end
  end
end