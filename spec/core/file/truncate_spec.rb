require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.truncate" do  
  before(:each) do
    @fname = "test.txt"
    @file  = File.open(@fname, 'w')
    File.open(@fname,"w"){ |f| f.write("1234567890") }
  end
 
  it "truncate the a file" do 
    File.open(@fname, "w") { |f| f.puts "123456789" } 
    if WINDOWS
      File.size(@fname).should == 11
    else
      File.size(@fname).should == 10
    end
    File.truncate(@fname, 5)
    File.size(@fname).should == 5
    File.open(@fname, "r") do |f|
      f.read(99).should == "12345"
      f.eof?.should == true
    end
  end  

  it "truncate to 0 a file" do
    File.truncate(@fname, 0).should == 0    
    IO.read(@fname).should == ""
  end
 
  it "truncate to 5 a file"  do
    File.size(@fname).should == 10
    File.truncate(@fname, 5) 
    File.size(@fname).should == 5
    IO.read(@fname).should == "12345"
  end

  it "truncate to a larger size than the original file" do
    File.truncate(@fname, 12) 
    File.size(@fname).should == 12
    IO.read(@fname).should == "1234567890\000\000"
  end

  it "truncate to a the same size the original file" do
    File.truncate(@fname, File.size(@fname))    
    File.size(@fname).should == 10
    IO.read(@fname).should == "1234567890"
  end
   
  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    should_raise(ArgumentError){ File.truncate(@fname) }
    should_raise(Errno::EINVAL){ File.truncate(@fname, -1) } # May fail
    should_raise(TypeError){ File.truncate(@fname, nil) }
  end

  after :each do
    @file.close rescue nil
    @fname = nil
  end  
end
