require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Encoding.default_internal" do
    before(:each) do
      @original_encoding = Encoding.default_internal
    end

    after(:each) do
      Encoding.default_internal = @original_encoding
    end

    it "is nil by default" do
      Encoding.default_internal.should be_nil
    end

    it "returns an Encoding object if a default internal encoding is set" do
      Encoding.default_internal = Encoding::ASCII
      Encoding.default_internal.should be_an_instance_of(Encoding)
    end

    it "returns nil if no default internal encoding is set" do
      Encoding.default_internal = nil
      Encoding.default_internal.should be_nil
    end

    it "returns the default internal encoding" do
      Encoding.default_internal = Encoding::ASCII_8BIT
      Encoding.default_internal.should == Encoding::ASCII_8BIT
    end

    it "returns Encoding::UTF_8 if ruby was invoked with -U" do
      ruby_exe("print Encoding.default_internal", :options => '-U').
        should == 'UTF-8'
    end

    it "uses the encoding specified when ruby is invoked with an '-E :internal' argument" do
      ruby_exe("print Encoding.default_internal", :options => '-E :SHIFT_JIS').
        should == 'Shift_JIS'
    end

    it "uses the encoding specified when ruby is invoked with an '-E external:internal' argument" do
      ruby_exe("print Encoding.default_internal", :options => '-E UTF-8:SHIFT_JIS').
        should == 'Shift_JIS'
    end
  end

  describe "Encoding.default_internal=" do
    before(:all) do
      @original_encoding = Encoding.default_internal
    end

    after(:all) do
      Encoding.default_internal = @original_encoding
    end

    it "sets the default internal encoding" do
      Encoding.default_internal = Encoding::SHIFT_JIS
      Encoding.default_internal.should == Encoding::SHIFT_JIS
    end

    it "can accept a name of an encoding as a String" do
      Encoding.default_internal = 'sjis'
      Encoding.default_internal.should == Encoding::SHIFT_JIS
    end

    it "calls #to_s on arguments that are neither Strings nor Encodings" do
      string = mock('string')
      string.should_receive(:to_str).twice.and_return('ascii')
      Encoding.default_internal = string
      Encoding.default_internal.should == Encoding::ASCII
    end

    it "raises a TypeError unless the argument is an Encoding or convertible to a String" do
      lambda { Encoding.default_internal = [] }.should raise_error(TypeError)
    end

    it "accepts an argument of nil to unset the default internal encoding" do
      lambda { Encoding.default_internal = nil }.should_not raise_error
    end
  end
end
