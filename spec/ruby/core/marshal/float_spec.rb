require File.expand_path('../../../spec_helper', __FILE__)

describe "Marshal.dump with Float" do
  it "represents NaN" do
    Marshal.dump(nan_value).should == "\004\bf\bnan"
  end

  it "represents +Infinity" do
    Marshal.dump(infinity_value).should == "\004\bf\binf"
  end

  it "represents -Infinity" do
    Marshal.dump(-infinity_value).should == "\004\bf\t-inf"
  end

  it "represents zero" do
    Marshal.dump(0.0).should == "\004\bf\0060"
  end

  ruby_version_is ""..."1.9" do
    it "represents a Float less than 1" do
      Marshal.dump(0.666666667).should == "\004\bf\e0.66666666699999999\000%u"
    end

    it "represents a Float greater than 1" do
      Marshal.dump(42.666666667).should == "\004\bf\03242.666666667000001\000\f\226"
    end
  end

  ruby_version_is "1.9" do
    it "represents a Float less than 1" do
      Marshal.dump(0.666666667).should == "\x04\bf\x100.666666667"
    end

    it "represents a Float much less than 1" do
      Marshal.dump(0.000000001234697).should == "\x04\bf\x101.234697e-9"
    end

    it "represents a Float greater than 1" do
      Marshal.dump(42.666666667).should == "\x04\bf\x1142.666666667"
    end

    it "represents a Float much greater than 1" do
      Marshal.dump(98743561239011.0).should == "\x04\bf\x1398743561239011"
    end

    it "represents a Float much greater than 1 with a very small fractional part" do
      Marshal.dump(799346183459.0000000002999312541).should == "\x04\bf\x11799346183459"
    end
  end
end
