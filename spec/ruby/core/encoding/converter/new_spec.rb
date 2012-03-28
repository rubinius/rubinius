require File.expand_path('../../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "Encoding::Converter.new" do
    it "accepts a String for the source encoding" do
      conv = Encoding::Converter.new("us-ascii", "utf-8")
      conv.source_encoding.should == Encoding::US_ASCII
    end

    it "accepts a String for the destination encoding" do
      conv = Encoding::Converter.new("us-ascii", "utf-8")
      conv.destination_encoding.should == Encoding::UTF_8
    end

    it "accepts an Encoding object for the source encoding" do
      conv = Encoding::Converter.new(Encoding::US_ASCII, "utf-8")
      conv.source_encoding.should == Encoding::US_ASCII
    end

    it "accepts an Encoding object for the destination encoding" do
      conv = Encoding::Converter.new("us-ascii", Encoding::UTF_8)
      conv.destination_encoding.should == Encoding::UTF_8
    end

    it "calls #to_str to convert the source encoding argument to an encoding name" do
      enc = mock("us-ascii")
      enc.should_receive(:to_str).and_return("us-ascii")
      conv = Encoding::Converter.new(enc, "utf-8")
      conv.source_encoding.should == Encoding::US_ASCII
    end

    it "calls #to_str to convert the destination encoding argument to an encoding name" do
      enc = mock("utf-8")
      enc.should_receive(:to_str).and_return("utf-8")
      conv = Encoding::Converter.new("us-ascii", enc)
      conv.destination_encoding.should == Encoding::UTF_8
    end
  end
end
