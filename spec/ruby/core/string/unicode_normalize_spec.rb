require File.expand_path('../../../spec_helper', __FILE__)

# Examples taken from http://www.unicode.org/reports/tr15/#Norm_Forms

describe "String#unicode_normalize" do
  before do
    @accented_f = "\u1e9b\u0323"
    @angstrom = "\u212b"
    @ohm = "\u2126"
  end

  it "normalizes code points in the string according to the form that is specified" do
    @accented_f.unicode_normalize(:nfc).should == "\u1e9b\u0323"
    @accented_f.unicode_normalize(:nfd).should == "\u017f\u0323\u0307"
    @accented_f.unicode_normalize(:nfkc).should == "\u1e69"
    @accented_f.unicode_normalize(:nfkd).should == "\u0073\u0323\u0307"
  end

  it "defaults to the nfc normalization form if no forms are specified" do
    @accented_f.unicode_normalize.should == "\u1e9b\u0323"
    @angstrom.unicode_normalize.should == "\u00c5"
    @ohm.unicode_normalize.should == "\u03a9"
  end

  it "raises an Encoding::CompatibilityError if the string is not in an unicode encoding" do
    lambda do
      @angstrom.force_encoding("ISO-8859-1").unicode_normalize
    end.should raise_error(Encoding::CompatibilityError)
  end

  it "raises an ArgumentError if the specified form is invalid" do
    lambda { @angstrom.unicode_normalize(:invalid_form) }.should raise_error(ArgumentError)
  end
end

describe "String#unicode_normalize!" do
  before do
    @ohm = "\u2126"
  end

  it "normalizes code points and modifies the receiving string" do
    angstrom = "\u212b"
    angstrom.unicode_normalize!
    angstrom.should == "\u00c5"
    angstrom.should_not == "\u212b"
  end

  it "raises an Encoding::CompatibilityError if the string is not in an unicode encoding" do
    lambda do
      @ohm.force_encoding("ISO-8859-1").unicode_normalize!
    end.should raise_error(Encoding::CompatibilityError)
  end

  it "raises an ArgumentError if the specified form is invalid" do
    lambda { @ohm.unicode_normalize!(:invalid_form) }.should raise_error(ArgumentError)
  end
end

describe "String#unicode_normalized?" do
  before do
    @nfc_normalized_str = "\u1e9b\u0323"
    @nfd_normalized_str = "\u017f\u0323\u0307"
    @nfkc_normalized_str = "\u1e69"
    @nfkd_normalized_str = "\u0073\u0323\u0307"
  end

  it "returns true if string is in the specified normalization form" do
    @nfc_normalized_str.unicode_normalized?(:nfc).should == true
    @nfd_normalized_str.unicode_normalized?(:nfd).should == true
    @nfkc_normalized_str.unicode_normalized?(:nfkc).should == true
    @nfkd_normalized_str.unicode_normalized?(:nfkd).should == true
  end

  it "returns false if string is not in the supplied normalization form" do
    @nfd_normalized_str.unicode_normalized?(:nfc).should == false
    @nfc_normalized_str.unicode_normalized?(:nfd).should == false
    @nfc_normalized_str.unicode_normalized?(:nfkc).should == false
    @nfc_normalized_str.unicode_normalized?(:nfkd).should == false
  end

  it "defaults to the nfc normalization form if no forms are specified" do
    @nfc_normalized_str.unicode_normalized?.should == true
    @nfd_normalized_str.unicode_normalized?.should == false
  end

  it "returns true if string is empty" do
    "".unicode_normalized?.should == true
  end

  it "returns true if string does not contain any unicode codepoints" do
    "abc".unicode_normalized?.should == true
  end

  it "raises an Encoding::CompatibilityError if the string is not in an unicode encoding" do
    lambda do
      @nfc_normalized_str.force_encoding("ISO-8859-1").unicode_normalized?
    end.should raise_error(Encoding::CompatibilityError)
  end

  it "raises an ArgumentError if the specified form is invalid" do
    lambda { @nfc_normalized_str.unicode_normalized?(:invalid_form) }.should raise_error(ArgumentError)
  end
end
