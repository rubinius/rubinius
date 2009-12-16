# coding: utf-8
require File.dirname(__FILE__) + '/shared/encode'

ruby_version_is "1.9" do
  describe "String#encode!" do
    it_behaves_like :encode_string, :encode!

    it "raises a RuntimeError when called on a frozen String" do
      lambda do
        "foo".freeze.encode!(Encoding::ANSI_X3_4_1968)
      end.should raise_error(RuntimeError)
    end

    # http://redmine.ruby-lang.org/issues/show/1836
    it "raises a RuntimeError when called on a frozen String when it's a no-op" do
      lambda do
        "foo".freeze.encode!("foo".encoding)
      end.should raise_error(RuntimeError)
    end

    it "returns self when called with only a target encoding" do
      str = "strung"
      str.encode!(Encoding::BINARY).should == str.force_encoding(Encoding::BINARY)
    end

    it "returns self when called with only a target encoding" do
      str = "strung"
      str.encode!(Encoding::BINARY,Encoding::ASCII).should == str.force_encoding(Encoding::BINARY)
    end

    it "returns self even when no changes are made" do
      str = "strung"
      str.encode!(Encoding::UTF_8).should == str
    end

    it "tags the String with the given encoding" do
      str = "ürst"
      str.encoding.should == Encoding::UTF_8
      str.encode!(Encoding::UTF_16LE)
      str.encoding.should == Encoding::UTF_16LE
    end
  end
end