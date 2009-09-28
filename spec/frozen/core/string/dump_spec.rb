# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#dump" do
  ruby_version_is ""..."1.9" do
    # Older versions of MRI wrongly print \b as \010
    it "returns a string with nonprinting charaters replaced by escaped-numeric notation" do
      ("\000".."A").to_a.to_s.dump.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
    end
  end

  # This test is different on 1.9 for two reasons: 1) 1.9 uses a different
  # format of escaping, 2) 1.9 changes the output of Array#to_s.
  ruby_version_is "1.9" do
    it "returns a string with nonprinting charaters replaced by \\x notation" do
      ("\000".."A").to_a.join('').should == "\x00\x01\x02\x03\x04\x05\x06\a\b\t\n\v\f\r\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\e\x1C\x1D\x1E\x1F !\"\#$%&'()*+,-./0123456789:;<=>?@A"
      # This test is based on my observations; the precise workings of this
      # feature are unknown to me
      it "includes .force_encoding(name) if the encoding isn't ASCII compatiable" do
        "\u{876}".encode('utf-16be').dump.should == 
            "\"\\bv\".force_encoding(\"UTF-16BE\")"
        "\u{876}".encode('utf-16le').dump.should == 
          "\"v\\b\".force_encoding(\"UTF-16LE\")"
      end
    end
  end

  # $KCODE is deprecated on 1.9
  ruby_version_is ""..."1.9" do
    it "ignores the $KCODE setting" do
      old_kcode = $KCODE

      begin
        $KCODE = "NONE"
        "äöü".dump.should == "\"\\303\\244\\303\\266\\303\\274\""

        $KCODE = "UTF-8"
        "äöü".dump.should == "\"\\303\\244\\303\\266\\303\\274\""
      ensure
        $KCODE = old_kcode
      end
    end
  end

  it "taints result when self is tainted" do
    "".taint.dump.tainted?.should == true
    "x".taint.dump.tainted?.should == true
  end
  
  it "returns a subclass instance for subclasses" do
    StringSpecs::MyString.new("hi!").dump.class.should == StringSpecs::MyString
  end
end
