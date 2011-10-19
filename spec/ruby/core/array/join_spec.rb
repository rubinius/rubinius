require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../fixtures/encoded_strings', __FILE__)
require File.expand_path('../shared/join', __FILE__)

describe "Array#join" do
  it_behaves_like :array_join, :join, ArraySpecs::NewArray
end

describe "Array#join" do
  it "does not separates elements when the passed separator is nil" do
    [1, 2, 3].join(nil).should == '123'
  end

  it "uses $, as the default separator (which defaults to nil)" do
    [1, 2, 3].join.should == '123'
    begin
      old, $, = $,, '-'
      [1, 2, 3].join.should == '1-2-3'
    ensure
      $, = old
    end
  end

  it "does not process the separator if the array is empty" do
    a = []
    sep = Object.new
    a.join(sep).should == ""
  end

  it "calls #to_str to convert the separator to a String" do
    sep = mock("separator")
    sep.should_receive(:to_str).and_return(", ")
    [1, 2].send(@method, sep).should == "1, 2"
  end

  it "calls #to_s on the Array elements" do
    a = mock("Array#join element")
    a.should_receive(:to_s).and_return("-a-")
    a.should_not_receive(:to_a)

    [a].join(",").should == "-a-"
  end

  it "raises a TypeError if the separator cannot be coerced to a String by calling #to_str" do
    obj = mock("not a string")
    lambda { [1, 2].send(@method, obj) }.should raise_error(TypeError)
  end

  ruby_version_is "1.9" do
    it "uses the first encoding when other strings are compatible" do
      ary1 = ArraySpecs.array_with_7bit_utf8_and_usascii_strings
      ary2 = ArraySpecs.array_with_usascii_and_7bit_utf8_strings
      ary3 = ArraySpecs.array_with_utf8_and_7bit_ascii8bit_strings
      ary4 = ArraySpecs.array_with_usascii_and_7bit_ascii8bit_strings

      ary1.join.encoding.should == Encoding::UTF_8
      ary2.join.encoding.should == Encoding::US_ASCII
      ary3.join.encoding.should == Encoding::UTF_8
      ary4.join.encoding.should == Encoding::US_ASCII
    end

    it "uses the widest common encoding when other strings are incompatible" do
      ary1 = ArraySpecs.array_with_utf8_and_usascii_strings
      ary2 = ArraySpecs.array_with_usascii_and_utf8_strings

      ary1.join.encoding.should == Encoding::UTF_8
      ary2.join.encoding.should == Encoding::UTF_8
    end

    it "fails for arrays with incompatibly-encoded strings" do
      ary_utf8_bad_ascii8bit = ArraySpecs.array_with_utf8_and_ascii8bit_strings
      
      lambda { ary_utf8_bad_ascii8bit.join }.should raise_error(EncodingError)
    end
  end
end
