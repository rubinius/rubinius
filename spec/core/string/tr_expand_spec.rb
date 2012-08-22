require File.expand_path('../../../spec_helper', __FILE__)

describe "String#tr_expand!" do
  it "replaces self with a string with all sequences expanded" do
    s = "a-g"
    s.tr_expand!(nil, true).should == 7
    s.should == "abcdefg"

    s = "abc-egh"
    s.tr_expand!(nil, true).should == 7
    s.should == "abcdegh"
  end

  it "returns duplicated characters in the last processed prosition" do
    s = "a-cabdage"
    s.tr_expand!(nil, true).should == 9
    s.should == "abcabdage"
  end

  it "handles 8-bit characters" do
    s = "\200\275\200"
    s.tr_expand!(nil, false).should == 3
    s.should == "\200\275\200"
  end

  it "handles entire range of characters" do
    s = "\x00-\xFF"
    s.tr_expand!(nil, false).should == 256
  end

  it "returns a string without leading '^' character if the string size is larger than 1" do
    s = "^"
    s.tr_expand!(nil, true).should == 1
    s.should == "^"

    s = "^a-c"
    s.tr_expand!(nil, true).should == 4
    s.should == "^abc"
  end

  it "accepts an optional Fixnum to limit the processing steps when expanding" do
    s = "a-h"
    s.tr_expand!(5, true) == 5
    s.should == "abcde"

    s = "abc-ga-i"
    s.tr_expand!(10, true).should == 10
    s.should == "abcdefgabc"

    s = "abc-ga-i"
    s.tr_expand!(3, true).should == 3
    s.should == "abc"

    s = "^abcde"
    s.tr_expand!(4, true).should == 4
    s.should == "^abc"
  end
end
