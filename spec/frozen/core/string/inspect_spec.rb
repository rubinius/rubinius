# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#inspect" do
  ruby_version_is ""..."1.9" do
    # Older versions of MRI wrongly print \b as \010
    it "returns a string with nonprinting charaters replaced by escaped-numeric notation" do
      ("\000".."A").to_a.to_s.inspect.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
    end
  end
  ruby_version_is "1.9" do
    it "returns a string with nonprinting charaters replaced by \\x notation" do
      ("\000".."A").to_a.join('').should == "\x00\x01\x02\x03\x04\x05\x06\a\b\t\n\v\f\r\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\e\x1C\x1D\x1E\x1F !\"\#$%&'()*+,-./0123456789:;<=>?@A"
    end
  end

  # $KCODE is deprecated/removed on 1.9
  ruby_version_is ""..."1.9" do
    it "produces different output based on $KCODE" do
      old_kcode = $KCODE

      begin
        $KCODE = "NONE"
        "äöü".inspect.should == "\"\\303\\244\\303\\266\\303\\274\""

        $KCODE = "UTF-8"
        "äöü".inspect.should == "\"äöü\""
      ensure
        $KCODE = old_kcode
      end
    end

    it "can handle malformed UTF-8 string when $KCODE is UTF-8" do
      old_kcode = $KCODE

      begin
        $KCODE = "UTF-8"
        # malformed UTF-8 sequence
        "\007äöüz\303".inspect.should == "\"\\aäöüz\\303\""
      ensure
        $KCODE = old_kcode
      end
    end
  end

  ruby_version_is "1.9" do
    it "produces different output based on #force_encoding" do
      "äöü".inspect.force_encoding('UTF-8').should == 
        "\"\xC3\xA4\xC3\xB6\xC3\xBC\""
      "äöü".inspect.force_encoding('ASCII').should == 
        "\"äöü\"".force_encoding('ASCII')
    end

    it "can handle malformed UTF-8 string for #force_encoding('UTF-8')" do
      # malformed UTF-8 sequence
      "\007äöüz\303".inspect.force_encoding('UTF-8').should == 
        "\"\\aäöüz\\xC3\"".force_encoding('UTF-8')
    end
  end

  it "taints the result if self is tainted" do
    "foo".taint.inspect.tainted?.should == true
    "foo\n".taint.inspect.tainted?.should == true
  end

  it "does not return subclass instances" do
    str = StringSpecs::MyString.new
    str << "test"
    str.should == "test"
    str.inspect.class.should_not == StringSpecs::MyString
  end
end
