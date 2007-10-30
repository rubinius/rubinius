require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#seek" do
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno
  end
  
  after :each do
    @file.close
  end
  
  it "moves the read position relative to the current position with SEEK_CUR" do
    @io.seek(10, IO::SEEK_CUR)
    @io.readline.should == "igne une.\n"
    @io.seek(-5, IO::SEEK_CUR)
    @io.readline.should == "une.\n"
  end
  
  it "moves the read position relative to the start with SEEK_SET" do
    @io.seek(42, IO::SEEK_SET)
    @io.readline.should == "quí está la línea tres.\n"
    @io.seek(5, IO::SEEK_SET)
    @io.readline.should == " la ligne une.\n"
  end
  
  it "moves the read position relative to the end with SEEK_END" do
    @io.seek(0, IO::SEEK_END)
    @io.tell.should == 134
    @io.seek(-25, IO::SEEK_END)
    @io.readline.should == "cinco.\n"
  end
end
