require File.expand_path('../../../spec_helper', __FILE__)

describe "String#tr_expand!" do
  it "replaces self with a string with all sequences expanded" do
    s = "a-g"
    s.tr_expand!(nil).should == 7
    s.should == "abcdefg"

    s = "abc-egh"
    s.tr_expand!(nil).should == 7
    s.should == "abcdegh"
  end

  it "returns duplicated characters in the last processed prosition" do
    s = "a-cabdage"
    s.tr_expand!(nil).should == 9
    s.should == "cbdage"
  end

  it "returns a string without leading '^' character if the string size is larger than 1" do
    s = "^"
    s.tr_expand!(nil).should == 1
    s.should == "^"

    s = "^a-c"
    s.tr_expand!(nil).should == 3
    s.should == "abc"
  end

  it "accepts an optional Fixnum to limit the processing steps when expanding" do
    s = "a-h"
    s.tr_expand!(5) == 5
    s.should == "abcde"

    s = "abc-ga-i"
    s.tr_expand!(10).should == 10
    s.should == "defgabc"

    s = "abc-ga-i"
    s.tr_expand!(3).should == 3
    s.should == "abc"

    s = "^abcde"
    s.tr_expand!(4).should == 4
    s.should == "abcd"
  end
end
