require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#inspect" do
  it "should produce a version of self with all nonprinting charaters replaced by \\nnn notation" do
    ("\000".."A").to_a.to_s.dump.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
  end
  
  it "should produce different output based on $KCODE" do
    old_kcode = $KCODE

    $KCODE = "NONE"
    "äöü".inspect.should == "\"\\303\\244\\303\\266\\303\\274\""

    $KCODE = "UTF-8"
    "äöü".inspect.should == "\"äöü\""
    
    $KCODE = old_kcode
  end
end