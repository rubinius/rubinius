require File.expand_path('../../../spec_helper', __FILE__)

describe "Regexp#options" do
  it "returns a Fixnum bitvector of regexp options for the Regexp object" do
    /cat/.options.should be_kind_of(Fixnum)
    /cat/ix.options.should be_kind_of(Fixnum)
  end

  it "allows checking for presence of a certain option with bitwise &" do
    (/cat/.options & Regexp::IGNORECASE).should == 0
    (/cat/i.options & Regexp::IGNORECASE).should_not == 0
    (/cat/.options & Regexp::MULTILINE).should == 0
    (/cat/m.options & Regexp::MULTILINE).should_not == 0
    (/cat/.options & Regexp::EXTENDED).should == 0
    (/cat/x.options & Regexp::EXTENDED).should_not == 0
    (/cat/mx.options & Regexp::MULTILINE).should_not == 0
    (/cat/mx.options & Regexp::EXTENDED).should_not == 0
    (/cat/xi.options & Regexp::IGNORECASE).should_not == 0
    (/cat/xi.options & Regexp::EXTENDED).should_not == 0
  end
end
