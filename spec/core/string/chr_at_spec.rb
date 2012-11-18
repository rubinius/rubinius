# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "String#chr_at" do
    before :each do
      @str = "∂φ/∂x = gaîté"
    end

    it "returns an instance of Rubinius::Character" do
      @str.chr_at(0).should be_an_instance_of(Rubinius::Character)
    end

    it "returns the encoded character at a byte offset" do
      char = @str.chr_at(15)
      char.bytesize.should == 2
      char.should == "î"
    end

    it "returns a character at byte 0" do
      @str.chr_at(0).should == "∂"
    end

    it "returns a character at the end of the String" do
      @str.chr_at(18).should == "é"
    end

    it "returns nil if there is no valid encoded character at the byte offset" do
      @str.chr_at(1).should be_nil
    end
  end
end
