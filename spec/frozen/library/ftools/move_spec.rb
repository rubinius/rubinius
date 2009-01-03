require File.dirname(__FILE__) + '/../../spec_helper'
require 'ftools'

describe "File.move" do
  before(:each) do
    system "echo 'hello rubinius' > move_test"
    system "chmod a+x move_test"
  end
  
  after(:each) do
    File.unlink "move_test_dest"
    File.unlink "move_test" rescue nil
  end
  
  it "moves the file at 1st arg to the file at 2nd arg" do
    omode = File.stat("move_test").mode
    File.move("move_test", "move_test_dest")
    fd = File.open("move_test_dest")
    data = fd.read
    data.should == "hello rubinius\n"
    fd.close
    mode = File.stat("move_test_dest").mode
    
    omode.should == mode
  end
end
