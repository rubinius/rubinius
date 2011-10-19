require File.expand_path('../../fixtures/encoded_strings', __FILE__)

describe :array_to_s, :shared => true do
  ruby_version_is "1.9" do
    before :each do
      @default_internal = Encoding.default_internal
    end
    
    after :each do
      Encoding.default_internal = @default_internal
    end
    
    it "uses default internal encoding regardless of strings' encodings or content" do
      Encoding.default_internal = Encoding::UTF_8
      
      ary_7bit_utf8_usascii = ArraySpecs.array_with_7bit_utf8_and_usascii_strings
      ary_usascii_7bit_utf8 = ArraySpecs.array_with_usascii_and_7bit_utf8_strings
      ary_utf8_usascii      = ArraySpecs.array_with_utf8_and_usascii_strings
      ary_utf8_ok_ascii8bit = ArraySpecs.array_with_utf8_and_7bit_ascii8bit_strings
      ary_utf8_bad_ascii8bit = ArraySpecs.array_with_utf8_and_ascii8bit_strings
      ary_usascii_ok_ascii8bit = ArraySpecs.array_with_usascii_and_7bit_ascii8bit_strings
      ary_usascii_bad_ascii8bit = ArraySpecs.array_with_usascii_and_ascii8bit_strings
      
      ary_7bit_utf8_usascii.send(@method).encoding.should == Encoding::UTF_8
      ary_usascii_7bit_utf8.send(@method).encoding.should == Encoding::UTF_8
      ary_utf8_usascii.send(@method).encoding.should == Encoding::UTF_8
      ary_utf8_ok_ascii8bit.send(@method).encoding.should == Encoding::UTF_8
      ary_utf8_bad_ascii8bit.send(@method).encoding.should == Encoding::UTF_8
      ary_usascii_ok_ascii8bit.send(@method).encoding.should == Encoding::UTF_8
      ary_usascii_bad_ascii8bit.send(@method).encoding.should == Encoding::UTF_8
      
      Encoding.default_internal = Encoding::US_ASCII
      
      ary_7bit_utf8_usascii = ArraySpecs.array_with_7bit_utf8_and_usascii_strings
      ary_usascii_7bit_utf8 = ArraySpecs.array_with_usascii_and_7bit_utf8_strings
      ary_utf8_usascii      = ArraySpecs.array_with_utf8_and_usascii_strings
      ary_utf8_ok_ascii8bit = ArraySpecs.array_with_utf8_and_7bit_ascii8bit_strings
      ary_utf8_bad_ascii8bit = ArraySpecs.array_with_utf8_and_ascii8bit_strings
      ary_usascii_ok_ascii8bit = ArraySpecs.array_with_usascii_and_7bit_ascii8bit_strings
      ary_usascii_bad_ascii8bit = ArraySpecs.array_with_usascii_and_ascii8bit_strings
      
      ary_7bit_utf8_usascii.send(@method).encoding.should == Encoding::US_ASCII
      ary_usascii_7bit_utf8.send(@method).encoding.should == Encoding::US_ASCII
      ary_utf8_usascii.send(@method).encoding.should == Encoding::US_ASCII
      ary_utf8_ok_ascii8bit.send(@method).encoding.should == Encoding::US_ASCII
      ary_utf8_bad_ascii8bit.send(@method).encoding.should == Encoding::US_ASCII
      ary_usascii_ok_ascii8bit.send(@method).encoding.should == Encoding::US_ASCII
      ary_usascii_bad_ascii8bit.send(@method).encoding.should == Encoding::US_ASCII
      
      Encoding.default_internal = Encoding::ASCII_8BIT
      
      ary_7bit_utf8_usascii = ArraySpecs.array_with_7bit_utf8_and_usascii_strings
      ary_usascii_7bit_utf8 = ArraySpecs.array_with_usascii_and_7bit_utf8_strings
      ary_utf8_usascii      = ArraySpecs.array_with_utf8_and_usascii_strings
      ary_utf8_ok_ascii8bit = ArraySpecs.array_with_utf8_and_7bit_ascii8bit_strings
      ary_utf8_bad_ascii8bit = ArraySpecs.array_with_utf8_and_ascii8bit_strings
      ary_usascii_ok_ascii8bit = ArraySpecs.array_with_usascii_and_7bit_ascii8bit_strings
      ary_usascii_bad_ascii8bit = ArraySpecs.array_with_usascii_and_ascii8bit_strings
      
      ary_7bit_utf8_usascii.send(@method).encoding.should == Encoding::ASCII_8BIT
      ary_usascii_7bit_utf8.send(@method).encoding.should == Encoding::ASCII_8BIT
      ary_utf8_usascii.send(@method).encoding.should == Encoding::ASCII_8BIT
      ary_utf8_ok_ascii8bit.send(@method).encoding.should == Encoding::ASCII_8BIT
      ary_utf8_bad_ascii8bit.send(@method).encoding.should == Encoding::ASCII_8BIT
      ary_usascii_ok_ascii8bit.send(@method).encoding.should == Encoding::ASCII_8BIT
      ary_usascii_bad_ascii8bit.send(@method).encoding.should == Encoding::ASCII_8BIT
    end
  end
end