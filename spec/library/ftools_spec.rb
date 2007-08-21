require File.dirname(__FILE__) + '/../spec_helper'
require 'ftools'

describe "File.catname" do
  it "should return the 2nd arg if it's not a directory" do
    File.catname("blah", "foo").should == "foo"    
  end
  
  it "should use File.join with the args" do
    File.catname("passwd", ".").should == "./passwd"
  end
  
  it "should use File.basename on the 1st arg before joining" do
    File.catname("etc/passwd", ".").should == "./passwd"
  end  
end

describe "File.syscopy" do
  
  before(:each) do
    system "echo 'hello rubinius' > syscopy_test"
    system "chmod a+x syscopy_test"
  end
  
  after(:each) do
    File.unlink "syscopy_test"
    File.unlink "syscopy_test_dest"
  end
  
  it "should copy the file at 1st arg to the file at 2nd arg" do
    
    File.syscopy("syscopy_test", "syscopy_test_dest")
    fd = File.open("syscopy_test_dest")
    fd.read.should == "hello rubinius\n"
    fd.clone
    
    omode = File.stat("syscopy_test").mode
    mode = File.stat("syscopy_test_dest").mode
    
    omode.should == mode
  end
  
  
end
