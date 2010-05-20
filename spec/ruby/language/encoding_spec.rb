require File.expand_path('../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "The __ENCODING__ pseudo-variable" do
    it "is an instance of Encoding" do
      __ENCODING__.should be_kind_of(Encoding)
    end

    it "is US-ASCII by default" do
      __ENCODING__.should == Encoding::US_ASCII
    end

    it "is the evaluated strings's one inside an eval" do
      eval("__ENCODING__".force_encoding("US-ASCII")).should == Encoding::US_ASCII
      eval("__ENCODING__".force_encoding("ASCII-8BIT")).should == Encoding::ASCII_8BIT
    end

    it "is equal to the specified encoding when a magic comment exists" do
      code = "# coding: ASCII-8BIT\n__ENCODING__".force_encoding("US-ASCII")
      eval(code).should == Encoding::ASCII_8BIT

      code = "# coding: US-ASCII\n__ENCODING__".force_encoding("ASCII-8BIT")
      eval(code).should == Encoding::US_ASCII
    end

    it "raises a SyntaxError if assigned to" do
      lambda { eval("__ENCODING__ = 1") }.should raise_error(SyntaxError)
    end
  end
end
