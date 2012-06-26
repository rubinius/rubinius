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

  ruby_version_is "1.9" do
    it "includes Regexp::FIXEDENCODING for a Regexp literal with the 'u' option" do
      (//u.options & Regexp::FIXEDENCODING).should_not == 0
    end

    it "includes Regexp::FIXEDENCODING for a Regexp literal with the 'e' option" do
      (//e.options & Regexp::FIXEDENCODING).should_not == 0
    end

    it "includes Regexp::FIXEDENCODING for a Regexp literal with the 's' option" do
      (//s.options & Regexp::FIXEDENCODING).should_not == 0
    end

    it "does not include Regexp::FIXEDENCODING for a Regexp literal with the 'n' option" do
      (//n.options & Regexp::FIXEDENCODING).should == 0
    end

    it "includes Regexp::NOENCODING for a Regexp literal with the 'n' option" do
      (//n.options & Regexp::NOENCODING).should_not == 0
    end
  end
end
