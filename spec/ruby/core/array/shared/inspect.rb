require File.expand_path('../../fixtures/encoded_strings', __FILE__)

describe :array_inspect, :shared => true do
  it "returns a string" do
    [1, 2, 3].send(@method).should be_kind_of(String)
  end

  it "returns '[]' for an empty Array" do
    [].send(@method).should == "[]"
  end

  ruby_version_is "1.9" do
    it "returns a US-ASCII string for an empty Array" do
      [].send(@method).encoding.should == Encoding::US_ASCII
    end
  end

  it "calls inspect on its elements and joins the results with commas" do
    items = Array.new(3) do |i|
      obj = mock(i.to_s)
      obj.should_receive(:inspect).and_return(i.to_s)
      obj
    end
    items.send(@method).should == "[0, 1, 2]"
  end

  it "represents a recursive element with '[...]'" do
    ArraySpecs.recursive_array.send(@method).should == "[1, \"two\", 3.0, [...], [...], [...], [...], [...]]"
    ArraySpecs.head_recursive_array.send(@method).should == "[[...], [...], [...], [...], [...], 1, \"two\", 3.0]"
    ArraySpecs.empty_recursive_array.send(@method).should == "[[...]]"
  end

  it "taints the result if the Array is non-empty and tainted" do
    [1, 2].taint.send(@method).tainted?.should be_true
  end

  it "does not taint the result if the Array is tainted but empty" do
    [].taint.send(@method).tainted?.should be_false
  end

  it "taints the result if an element is tainted" do
    ["str".taint].send(@method).tainted?.should be_true
  end

  ruby_version_is "1.9" do
    it "untrusts the result if the Array is untrusted" do
      [1, 2].untrust.send(@method).untrusted?.should be_true
    end

    it "does not untrust the result if the Array is untrusted but empty" do
      [].untrust.send(@method).untrusted?.should be_false
    end

    it "untrusts the result if an element is untrusted" do
      ["str".untrust].send(@method).untrusted?.should be_true
    end
  end

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
