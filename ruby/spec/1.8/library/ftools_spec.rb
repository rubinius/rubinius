require File.dirname(__FILE__) + '/../spec_helper'
require 'ftools'

describe "File.catname" do
  it "returns the 2nd arg if it's not a directory" do
    File.catname("blah", "/etc/passwd").should == "/etc/passwd"
  end
  
  it "uses File.join with the args" do
    File.catname("passwd", ".").should == "./passwd"
  end
  
  it "uses File.basename on the 1st arg before joining" do
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
    File.unlink "syscopy_test_dest" rescue nil
  end
  
  it "copies the file at 1st arg to the file at 2nd arg" do
    File.syscopy("syscopy_test", "syscopy_test_dest")
    fd = File.open("syscopy_test_dest")
    data = fd.read
    data.should == "hello rubinius\n"
    fd.close
    
    omode = File.stat("syscopy_test").mode
    mode = File.stat("syscopy_test_dest").mode
    
    omode.should == mode
  end
end
