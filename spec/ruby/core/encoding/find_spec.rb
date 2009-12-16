require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Encoding.find" do
    before(:all) do
      @encodings = Encoding.aliases.to_a.flatten.uniq
    end

    it "returns the corresponding Encoding object if given a valid encoding name" do
      @encodings.each do |enc|
        Encoding.find(enc).should be_an_instance_of(Encoding)
      end
    end

    it "returns the corresponding Encoding object if given a valid alias name" do
      Encoding.aliases.keys.each do |enc_alias|
        Encoding.find(enc_alias).should be_an_instance_of(Encoding)
      end
    end

    it "accepts encoding names as Symbols" do
      @encodings.each do |enc|
        Encoding.find(enc.to_sym).should == Encoding.find(enc)
      end
    end

    it "accepts encoding names as Strings" do
      Encoding.list.each do |enc|
        Encoding.find(enc.name).should == enc
      end
    end

    it "accepts any object as encoding name, if it responds to #to_str" do
      obj = Class.new do
        attr_writer :encoding_name
        def to_str; @encoding_name; end
      end.new

      Encoding.list.each do |enc|
        obj.encoding_name = enc.name
        Encoding.find(obj).should == enc
      end
    end

    it "is case insensitive" do
      @encodings.each do |enc|
        Encoding.find(enc.upcase).should == Encoding.find(enc)
      end
    end

    it "raises an ArgumentError if the given encoding does not exist" do
      lambda { Encoding.find('dh2dh278d') }.should raise_error(ArgumentError)
    end
  end
end