require File.expand_path('../../../spec_helper', __FILE__)

describe "String#lstrip" do
  it "returns a string with all leading \\000 and whitespace characters removed" do
    "".lstrip.should == ""
    " hello ".lstrip.should == "hello "
    "\000  hello ".lstrip.should == "hello "
    "\000\t \000hello ".lstrip.should == "hello "
    "hello".lstrip.should == "hello"
  end

  it "modifies self by removing all leading \\000 and whitespace characters" do
    "\n\t This \000".lstrip!.should == "This \000"
    "\000 another one".lstrip!.should == "another one"
    "\000  \000\t\v\000two  ".lstrip!.should == "two  "
  end
end
