require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#getch" do
  before :each do
    @kcode = $KCODE
  end

  after :each do
    $KCODE = @kcode
  end

  it "scans one character and returns it" do
    s = StringScanner.new('abc')
    s.getch.should == "a"
    s.getch.should == "b"
    s.getch.should == "c"
  end

  it "is multi-byte character sensitive" do
    $KCODE = 'EUC'

    # Japanese hiragana "A" in EUC-JP
    src = encode("\244\242", "euc-jp")

    s = StringScanner.new(src)
    s.getch.should == src
  end

  it "returns nil at the end of the string" do
    # empty string case
    s = StringScanner.new('')
    s.getch.should == nil
    s.getch.should == nil

    # non-empty string case
    s = StringScanner.new('a')
    s.getch # skip one
    s.getch.should == nil
  end

  it "does not accept any arguments" do
    s = StringScanner.new('abc')
    lambda {
      s.getch(5)
    }.should raise_error(ArgumentError, /wrong .* arguments/)
  end
end
