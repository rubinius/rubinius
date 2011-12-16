require File.expand_path('../spec_helper', __FILE__)

ruby_version_is "1.9" do
  load_extension('encoding')

  describe "C-API Encoding function" do
    before :each do
      @s = CApiEncodingSpecs.new
    end

    describe "rb_usascii_encoding" do
      it "returns the encoding for Encoding::US_ASCII" do
        @s.rb_usascii_encoding.should == "US-ASCII"
      end
    end

    describe "rb_ascii8bit_encoding" do
      it "returns the encoding for Encoding::ASCII_8BIT" do
        @s.rb_ascii8bit_encoding.should == "ASCII-8BIT"
      end
    end

    describe "rb_utf8_encoding" do
      it "returns the encoding for Encoding::UTF_8" do
        @s.rb_utf8_encoding.should == "UTF-8"
      end
    end
  end
end
