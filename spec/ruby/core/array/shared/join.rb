require File.expand_path('../../fixtures/classes', __FILE__)
require File.expand_path('../../fixtures/encoded_strings', __FILE__)

describe :array_join_with_default_separator, :shared => true do
  before do
    @separator = $,
  end

  after do
    $, = @separator
  end

  it "returns an empty string if the Array is empty" do
    [].send(@method).should == ''
  end

  ruby_version_is "1.9" do
    it "returns a US-ASCII string for an empty Array" do
      [].send(@method).encoding.should == Encoding::US_ASCII
    end
  end

  ruby_version_is ""..."1.9" do
    it "returns a string formed by concatenating each element.to_s separated by $," do
      $, = " | "
      obj = mock('foo')
      obj.should_receive(:to_s).and_return("foo")
      [1, 2, 3, 4, obj].send(@method).should == '1 | 2 | 3 | 4 | foo'
    end
  end

  ruby_version_is "1.9" do
    it "returns a string formed by concatenating each element.to_str separated by $," do
      $, = " | "
      obj = mock('foo')
      obj.should_receive(:to_str).and_return("foo")
      [1, 2, 3, 4, obj].send(@method).should == '1 | 2 | 3 | 4 | foo'
    end
  end

  it "raises a NoMethodError if an element does not respond to #to_s" do
    obj = mock('o')
    class << obj; undef :to_s; end
    lambda { [1, obj].send(@method) }.should raise_error(NoMethodError)
  end

  ruby_version_is "".."1.9" do
    ruby_bug "[ruby-dev:37019]", "1.8.6.319" do
      it "represents a recursive element with '[...]'" do
        ArraySpecs.recursive_array.send(@method).should == "1two3.0[...][...][...][...][...]"
        ArraySpecs.head_recursive_array.send(@method).should == "[...][...][...][...][...]1two3.0"
        ArraySpecs.empty_recursive_array.send(@method).should == "[...]"
      end
    end
  end

  ruby_version_is "1.9" do
    it "raises an ArgumentError when the Array is recursive" do
      lambda { ArraySpecs.recursive_array.send(@method) }.should raise_error(ArgumentError)
      lambda { ArraySpecs.head_recursive_array.send(@method) }.should raise_error(ArgumentError)
      lambda { ArraySpecs.empty_recursive_array.send(@method) }.should raise_error(ArgumentError)
    end
  end

  it "taints the result if the Array is tainted and non-empty" do
    [1, 2].taint.send(@method).tainted?.should be_true
  end

  it "does not taint the result if the Array is tainted but empty" do
    [].taint.send(@method).tainted?.should be_false
  end

  it "taints the result if an element is tainted" do
    ["str".taint].send(@method).tainted?.should be_true
  end

  ruby_version_is "1.9" do
    it "untrusts the result if the Array is untrusted and non-empty" do
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
    it "uses the first encoding when other strings are compatible" do
      ary1 = ArraySpecs.array_with_7bit_utf8_and_usascii_strings
      ary2 = ArraySpecs.array_with_usascii_and_7bit_utf8_strings
      ary3 = ArraySpecs.array_with_utf8_and_7bit_ascii8bit_strings
      ary4 = ArraySpecs.array_with_usascii_and_7bit_ascii8bit_strings

      ary1.send(@method).encoding.should == Encoding::UTF_8
      ary2.send(@method).encoding.should == Encoding::US_ASCII
      ary3.send(@method).encoding.should == Encoding::UTF_8
      ary4.send(@method).encoding.should == Encoding::US_ASCII
    end

    it "uses the widest common encoding when other strings are incompatible" do
      ary1 = ArraySpecs.array_with_utf8_and_usascii_strings
      ary2 = ArraySpecs.array_with_usascii_and_utf8_strings

      ary1.send(@method).encoding.should == Encoding::UTF_8
      ary2.send(@method).encoding.should == Encoding::UTF_8
    end

    it "fails for arrays with incompatibly-encoded strings" do
      ary_utf8_bad_ascii8bit = ArraySpecs.array_with_utf8_and_ascii8bit_strings

      lambda { ary_utf8_bad_ascii8bit.send(@method) }.should raise_error(EncodingError)
    end
  end
end

describe :array_join_with_string_separator, :shared => true do
  ruby_version_is ""..."1.9" do
    it "returns a string formed by concatenating each element.to_s separated by separator" do
      obj = mock('foo')
      obj.should_receive(:to_s).and_return("foo")
      [1, 2, 3, 4, obj].send(@method, ' | ').should == '1 | 2 | 3 | 4 | foo'
    end
  end

  ruby_version_is "1.9" do
    it "returns a string formed by concatenating each element.to_str separated by separator" do
      obj = mock('foo')
      obj.should_receive(:to_str).and_return("foo")
      [1, 2, 3, 4, obj].send(@method, ' | ').should == '1 | 2 | 3 | 4 | foo'
    end
  end

  it "uses the same separator with nested arrays" do
    [1, [2, [3, 4], 5], 6].send(@method, ":").should == "1:2:3:4:5:6"
    [1, [2, ArraySpecs::MyArray[3, 4], 5], 6].send(@method, ":").should == "1:2:3:4:5:6"
  end

  it "taints the result if the separator is tainted and the array is non-empty" do
    [1, 2].send(@method, ":".taint).tainted?.should be_true
  end

  it "does not taint the result if the separator is tainted but the array is empty" do
    [].send(@method, ":".taint).tainted?.should be_false
  end

  ruby_version_is '1.9' do
    it "untrusts the result if the separator is untrusted and the array is non-empty" do
      [1, 2].send(@method, ":".untrust).untrusted?.should be_true
    end

    it "does not untrust the result if the separator is untrusted but the array is empty" do
      [].send(@method, ":".untrust).untrusted?.should be_false
    end
  end
end
