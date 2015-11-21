require File.expand_path('../../../spec_helper', __FILE__)

describe "String#unicode_normalize" do
  before :each do
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
    lambda { @angstrom.force_encoding("ISO-8859-1").unicode_normalize }.should raise_error(Encoding::CompatibilityError)
  end

  it "raises an ArgumentError if the specified form is invalid" do
    lambda { @angstrom.unicode_normalize(:invalid_form) }.should raise_error(ArgumentError)
  end
end

describe "String#unicode_normalize!" do
  before :each do
    @ohm = "\u2126"
  end

  it "normalizes code points and modifies the receiving string" do
    angstrom = "\u212b"
    angstrom.unicode_normalize!
    angstrom.should == "\u00c5"
    angstrom.should_not == "\u212b"
  end

  it "raises an Encoding::CompatibilityError if the string is not in an unicode encoding" do
    lambda { @ohm.force_encoding("ISO-8859-1").unicode_normalize! }.should raise_error(Encoding::CompatibilityError)
  end

  it "raises an ArgumentError if the specified form is invalid" do
    lambda { @ohm.unicode_normalize!(:invalid_form) }.should raise_error(ArgumentError)
  end
end
end
