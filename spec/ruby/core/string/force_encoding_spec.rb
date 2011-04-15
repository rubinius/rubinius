require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "String#force_encoding" do
    it "requires exactly one argument" do
      lambda {
        "glark".force_encoding
      }.should  raise_error(ArgumentError)

      lambda {
        "glark".force_encoding('h','d')
      }.should raise_error(ArgumentError)
    end

    it "accepts the encoding name as a String" do
      lambda {
        str.force_encoding('shift_jis')
      }.should_not raise_error(ArgumentError)
    end

    it "accepts the encoding name as an Encoding object" do
      lambda {
        str.force_encoding(Encoding::SHIFT_JIS)
      }.should_not raise_error(ArgumentError)
    end

    it "tags the String with the given encoding" do
      str = "a"
      str.encoding.should == Encoding::US_ASCII
      str.force_encoding(Encoding::SHIFT_JIS)
      str.encoding.should == Encoding::SHIFT_JIS
    end

    it "returns self" do
      str = "glark"
      id = str.object_id
      str.force_encoding('utf-8').object_id.should == id
    end

    it "does not care if self would be invalid in given encoding" do
      str = "\u{9765}"
      str.force_encoding('euc-jp')
      str.encoding.should == Encoding::EUC_JP
      str.valid_encoding?.should be_false
    end

    it "does not care if self is already tagged with the given encoding" do
      str = "\u{9765}"
      str.encoding.should == Encoding::UTF_8
      lambda {
        str.force_encoding('utf-8')
      }.should_not raise_error(ArgumentError)
      str.encoding.should == Encoding::UTF_8
    end

    it "does not transcode self" do
      "\u{8612}".force_encoding('utf-16le').should_not ==
        "\u{8612}".encode('utf-16le')
    end
  end
end
