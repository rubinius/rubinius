require File.expand_path('../../../spec_helper', __FILE__)

describe "String#rstrip" do
  it "returns a string with all trailing \\000 and whitespace characters removed" do
    " \t\n ".rstrip.should == ""
    "\t".rstrip.rstrip.should == ""
    "".rstrip.rstrip.should == ""
    " hello ".rstrip.rstrip.should == " hello"
    "\tgoodbye\r\n".rstrip.rstrip.should == "\tgoodbye"
    "goodbye \000".rstrip.rstrip.should == "goodbye"
    "goodbye \000\t \f  \000".rstrip.rstrip.should == "goodbye" 
  end

  it "modifies self by removing all trailing \\000 and whitespace characters" do
    " hello ".rstrip!.should == " hello"
    "\tgoodbye\r\n".rstrip!.should == "\tgoodbye"
    "goodbye \000".rstrip!.should == "goodbye"
    "goodbye \000 ".rstrip!.should == "goodbye"
    "".rstrip!.should == nil
    " \n \000\v\000".rstrip!.should == ""
  end
end
