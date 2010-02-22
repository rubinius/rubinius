require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "Encoding.compatible?" do
    it "returns an Encoding object if the given encodings are compatible" do
      Encoding.compatible?('a','a').should be_an_instance_of(Encoding)
    end

    it "returns the encoding of the string that would result from concatenating the arguments" do
      [["\u{9876}".force_encoding('UTF-8'), "\u{1254}".force_encoding('UTF-8')],
      ["a".encode!('SJIS'), "b".encode!('ASCII')]].each do |pair|
        Encoding.compatible?(pair.first, pair.last).
          should == pair.first.concat(pair.last).encoding
      end
    end

    it "returns nil if there is not a compatible encoding for the arguments" do
       Encoding.compatible?("\xa1".force_encoding("iso-8859-1"),
                            "\xa1\xa1".force_encoding("euc-jp")).should be_nil
    end

    it "returns nil unless both arguments are Strings" do
      Encoding.compatible?([], :foo).should be_nil
    end
  end
end
