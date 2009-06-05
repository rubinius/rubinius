# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

ruby_version_is '1.9' do
  describe "String#ascii_only?" do
    it "needs to be reviewed for spec completeness"
    
    it "returns true if the String contains only ASCII characters" do
      "hello".ascii_only?.should be_true
    end
    
    it "returns true if the String contains only ASCII characters
    and is encoded as ASCII" do
      "hello".force_encoding('ASCII').ascii_only?.should be_true
      "hello".encode("ASCII").ascii_only?.should be_true
    end

    it "returns true if the String contains only ASCII characters
    and is encoded as UTF-8" do
      "hello".force_encoding('UTF-8').ascii_only?.should be_true
      "hello".encode('UTF-8').ascii_only?.should be_true
    end
    
    it "returns false if the String contains only non-ASCII characters" do
      "\u{6666}".ascii_only?.should be_false
    end
    
    it "returns false if the String contains only non-ASCII characters
    and is encoded as UTF-8" do
      "\u{6666}".force_encoding('UTF-8').ascii_only?.should be_false
      "\u{6666}".encode('UTF-8').ascii_only?.should be_false
    end
    
    it "returns false if the String contains only non-ASCII characters
    and is encoded as ASCII" do
      "\u{6666}".force_encoding('ASCII').ascii_only?.should be_false
    end
    
    it "returns false if the String contains ASCII and non-ASCII characters" do
      "hello, \u{6666}".ascii_only?.should be_false
    end

    it "returns false if the String contains ASCII and non-ASCII characters
    and is encoded as ASCII" do
      "hello, \u{6666}".force_encoding('ASCII').ascii_only?.should be_false
    end
    
    it "returns false if the String contains ASCII and non-ASCII characters
    and is encoded as UTF-8" do
      "hello, \u{6666}".encode('UTF-8').ascii_only?.should be_false
      "hello, \u{6666}".force_encoding('UTF-8').ascii_only?.should be_false
    end
    
    it "accepts no arguments" do
      lambda { "Glark".ascii_only?('?') }.should raise_error(ArgumentError)
    end
    
    it "returns true for the empty String" do
      ''.ascii_only?.should be_true
      ''.encode('UTF-8').ascii_only?.should be_true
    end
  end
end
