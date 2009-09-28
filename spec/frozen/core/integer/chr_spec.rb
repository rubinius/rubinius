require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do
  describe "Integer#chr" do
    it "returns a string containing the ASCII character represented by self" do
      [82.chr, 117.chr, 98.chr, 105.chr, 110.chr, 105.chr, 117.chr, 115.chr, 
       32.chr, 
       114.chr, 111.chr, 99.chr, 107.chr, 115.chr].should == 
        ["R", "u", "b", "i", "n", "i", "u", "s", " ", "r", "o", "c", "k", "s"]
    end

    it "returns a new String for each call" do
      82.chr.should_not equal(82.chr)
    end
    
    it "raises a RangeError if self is out of the ASCII character range" do
      lambda { -1.chr }.should raise_error(RangeError)
      lambda { -300.chr }.should raise_error(RangeError)
      lambda { (-bignum_value).chr }.should raise_error(RangeError)

      lambda { 300.chr }.should raise_error(RangeError)
      lambda { bignum_value.chr }.should raise_error(RangeError)
    end
  end
end

ruby_version_is "1.9" do
  describe "Integer#chr without argument" do
    it "returns a String" do
      17.chr.should be_an_instance_of(String)
    end

    it "returns a new String for each call" do
      82.chr.should_not equal(82.chr)
    end

    it "returns a US-ASCII String if self is between 0 and 127 (inclusive)" do
      0.chr.encoding.should == Encoding::US_ASCII
      69.chr.encoding.should == Encoding::US_ASCII
      127.chr.encoding.should == Encoding::US_ASCII
      128.chr.encoding.should_not == Encoding::US_ASCII
    end

    it "returns an ASCII-8BIT String if self is between 128 and 255 (inclusive)" do
      128.chr.encoding.should == Encoding::ASCII_8BIT
      210.chr.encoding.should == Encoding::ASCII_8BIT
      255.chr.encoding.should == Encoding::ASCII_8BIT
    end

    it "interprets self as a codepoint in the corresponding character set" do
      80.chr.should == 'P'.force_encoding(Encoding::US_ASCII)
      80.chr.codepoints.to_a.should == [80]

      200.chr.should == "\xC8".force_encoding(Encoding::ASCII_8BIT)
      200.chr.codepoints.to_a.should == [200]
    end

    it "raises a RangeError is self is greater than 255 and the internal encoding is nil" do
      Encoding.default_internal.should be_nil
      lambda { 256.chr }.should raise_error(RangeError)
      lambda { bignum_value.chr }.should raise_error(RangeError)
    end

    it "infers the encoding from Encoding.default_internal" do
      old_internal = Encoding.default_internal
      Encoding.default_internal = Encoding::UTF_8
      120818.chr.should == "\u{1d7f2}"
      Encoding.default_internal = old_internal
    end

    it "raises a RangeError is self is less than 0" do
      lambda { -1.chr }.should raise_error(RangeError)
      lambda { -bignum_value.chr }.should raise_error(RangeError)
    end
  end

  describe "Integer#chr with an encoding argument" do
    it "returns a String" do
      900.chr(Encoding::UTF_8).should be_an_instance_of(String)
    end

    it "returns a new String for each call" do
      8287.chr(Encoding::UTF_8).should_not equal(8287.chr(Encoding::UTF_8))
    end

    it "accepts an Encoding object as an argument" do
      lambda { 568.chr(Encoding::SHIFT_JIS) }.should_not raise_error
    end

    it "accepts a String as an argument" do
      lambda { 56.chr('euc-jp') }.should_not raise_error
    end

    it "converts a String to an Encoding as Encoding.find does" do
      ['utf-8', 'UTF-8', 'Utf-8'].each do |encoding|
        7894.chr(encoding).encoding.should == Encoding::UTF_8
      end
    end

    it "returns characters in the specified encoding even if they exist in US-ASCII" do
      97.chr.encoding.should == Encoding::US_ASCII
      97.chr(Encoding::UTF_8).encoding.should == Encoding::UTF_8
    end

    it "behaves as called with no argument if encoding is specified as US-ASCII and self is less than 128" do
      0.chr(Encoding::US_ASCII).should == 0.chr
      69.chr(Encoding::US_ASCII).should == 69.chr
      127.chr(Encoding::US_ASCII).should == 127.chr
    end

    it "behaves as called with no argument if encoding is specified as ASCII-8BIT and self is between 128 and 255" do
      128.chr(Encoding::ASCII_8BIT).should == 128.chr
      200.chr(Encoding::ASCII_8BIT).should == 200.chr
      255.chr(Encoding::ASCII_8BIT).should == 255.chr
      lambda { 256.chr(Encoding::ASCII_8BIT) }.should raise_error(RangeError)
    end

    it "interprets self as a codepoint in the corresponding character set" do
      0x56CE.chr(Encoding::UTF_8).should == "\u{56CE}"
      0x56CE.chr(Encoding::UTF_8).codepoints.to_a.should == [0x56CE]

      41900.chr(Encoding::BIG5).should == "\xA3\xAC".\
        force_encoding(Encoding::BIG5)
      41900.chr(Encoding::BIG5).codepoints.to_a.should == [41900]

      129.chr(Encoding::KOI8_R).should == "\x81".\
        force_encoding(Encoding::KOI8_R)
      129.chr(Encoding::KOI8_R).codepoints.to_a.should == [129]
    end

    it "raises a RangeError if self is an invalid codepoint for the given encoding" do
      lambda { 0x81.chr(Encoding::EUC_JP)     }.should raise_error(RangeError)
      lambda { 256.chr(Encoding::ISO_8859_9)  }.should raise_error(RangeError)
      lambda { 620.chr(Encoding::TIS_620)     }.should raise_error(RangeError)
    end

    it "raises a RangeError is self is less than 0" do
      lambda { -1.chr(Encoding::UTF_8)             }.\
        should raise_error(RangeError)
      lambda { -bignum_value.chr(Encoding::EUC_JP) }.\
        should raise_error(RangeError)
    end
  end
end
