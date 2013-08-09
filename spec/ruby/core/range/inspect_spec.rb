require File.expand_path('../../../spec_helper', __FILE__)

describe "Range#inspect" do
  it "provides a printable form, using #inspect to convert the start and end objects" do
    ('A'..'Z').inspect.should == '"A".."Z"'
    ('A'...'Z').inspect.should == '"A"..."Z"'

    (0..21).inspect.should == "0..21"
    (-8..0).inspect.should ==  "-8..0"
    (-411..959).inspect.should == "-411..959"
    (0xfff..0xfffff).inspect.should == "4095..1048575"
    (0.5..2.4).inspect.should == "0.5..2.4"
  end

  it "returns a tainted string if either end is tainted" do
    (("a".taint)..."c").inspect.tainted?.should be_true
    ("a"...("c".taint)).inspect.tainted?.should be_true
  end

  it "ignores own tainted status" do
    ("a"..."c").taint.inspect.tainted?.should be_false
  end

  ruby_version_is "1.9" do
    it "returns a untrusted string if either end is untrusted" do
      (("a".untrust)..."c").inspect.untrusted?.should be_true
      ("a"...("c".untrust)).inspect.untrusted?.should be_true
    end

    it "ignores own untrusted status" do
      ("a"..."c").untrust.inspect.untrusted?.should be_false
    end
  end
end
