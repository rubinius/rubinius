require File.dirname(__FILE__) + '/../spec_helper'

describe "Sprintf::Formatter instance method" do
  before(:each) do
    @formatter = Sprintf::Formatter.new
  end

  it "flags & flags= returns/sets the flag attribute" do
    @formatter.flags = 1
    @formatter.flags.should == 1
  end

  it "value & value= returns/sets the value attribute" do
    @formatter.value = 2
    @formatter.value.should == 2
    begin
      # value can only be set once
      @formatter.value = 2
      "No Error"
    rescue ArgumentError
      $!.message
    end.should == 'value given twice '
  end

  it "type & type= returns/sets the type attribute" do
    @formatter.type = 2
    @formatter.type.should == 2
    begin
      # type can only be set once
      @formatter.type = 2
      'No Error'
    rescue ArgumentError
      $!.message
    end.should == 'type given twice '
  end

  it "width & width= returns/sets the width attribute" do
    @formatter.width = 2
    @formatter.width.should == 2
    begin
      # width can only be set once
      @formatter.width = 2
      'No Error'
    rescue ArgumentError
      $!.message
    end.should == 'width given twice '
  end

  it "precision & precision= returns/sets the precision attribute" do
    @formatter.precision = 2
    @formatter.precision.should == 2
    begin
      # precision can only be set once
      @formatter.precision = 2
      'No Error'
    rescue ArgumentError
      $!.message
    end.should == 'precision given twice'
  end

  it "fill returns the string padded to width" do
    @formatter.width = 10
    @formatter.fill("hello").should == "     hello"
    @formatter.flags = Sprintf::Formatter::PAD_LJUSTIFY
    @formatter.fill("hello").should == "hello     "
    @formatter.fill("12345678901").should == "12345678901"
  end

  it "truncate returns the string truncated to precision" do
    @formatter.truncate("hello").should == "hello"
    @formatter.precision = 3
    @formatter.truncate("hello").should == "hel"
  end

  it "radix returns the radix appended to the string" do
    @formatter.type = ?b
    @formatter.radix("1").should == '1'
    @formatter.flags = Sprintf::Formatter::PAD_RADIX
    @formatter.radix("1").should == '0b1'
  end

  it "radix returns the uppercase binary radix appended to the string" do
    @formatter.type = ?B
    @formatter.flags = Sprintf::Formatter::PAD_RADIX
    @formatter.radix("1").should == '0B1'
  end

  it "radix returns the octal radix appended to the string" do
    @formatter.type = ?o
    @formatter.flags = Sprintf::Formatter::PAD_RADIX
    @formatter.radix("1").should == "01"
  end

  it "radix returns the hexadecimal radix appended to the string" do
    @formatter.type = ?x
    @formatter.flags = Sprintf::Formatter::PAD_RADIX
    @formatter.radix("1").should == "0x1"
  end

  it "radix returns the uppercase hexadecimal radix appended to the string" do
    @formatter.type = ?X
    @formatter.flags = Sprintf::Formatter::PAD_RADIX
    @formatter.radix("1").should == "0X1"
  end

  it "radix returns appends no radix for decimal and floating point" do
    @formatter.type = ?d
    @formatter.flags = Sprintf::Formatter::PAD_RADIX
    @formatter.radix("1").should == "1"
  end

  it "onespad returns string left padded with digit relevant to the number base" do
    @formatter.onespad("0", 2).should == '0'
    @formatter.flags = Sprintf::Formatter::PAD_ZERO
    @formatter.width = 10
    @formatter.onespad("0", 2).should == "1111111110"
    @formatter.precision = 5
    @formatter.onespad("0", 2).should == "11110"
  end

  it "onespad returns octal string left padded with max octal digit" do
    @formatter.precision = 5
    @formatter.onespad("0", 8).should == "77770"
  end

  it "onespad returns decimal string left padded with max decimal digit" do
    @formatter.precision = 5
    @formatter.onespad("0", 10).should == "....0"
  end

  it "onespad returns hex string left padded with max hex digit" do
    @formatter.precision = 5
    @formatter.onespad("0", 16).should == "ffff0"
  end

  it "zeropad returns str left padded with 0" do
     @formatter.zeropad("aaaa", 4).should == "aaaa"
    @formatter.zeropad("aaaa", 5).should == "0aaaa"
  end

  it "sign returns signed string" do
    @formatter.sign("1", 1).should == '1'
    @formatter.flags = Sprintf::Formatter::PAD_PLUS
    @formatter.sign("1", 1).should == '+1'
    @formatter.flags = Sprintf::Formatter::PAD_SPACE
    @formatter.sign("1", 1).should == ' 1'
    @formatter.sign("-1", -1).should == '-1'
  end
end
