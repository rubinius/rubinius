require File.dirname(__FILE__) + '/../../../spec_helper'

describe "String#%" do
  it "formats float values Infinity and Nan" do
    ("%E" % 1e1020).should == "Inf"
    ("%E" % -1e1020).should == "-Inf"
    ("%-10E" % 1e1020).should == "Inf       "
    ("%-010E" % 1e1020).should == "0000000Inf"
    ("%010E" % 1e1020).should == "0000000Inf"
    ("%+E" % 1e1020).should == "+Inf"
    ("% E" % 1e1020).should == " Inf"
    ("%E" % (0.0/0)).should == "NAN"
    ("%E" % (-0e0/0)).should == "NAN"
  end

  it "formats float (e) values should return a string resulting from applying the format" do  
    ("%e" % 1e1020).should == "inf"
    ("%e" % -1e1020).should == "-inf"
  end
end
