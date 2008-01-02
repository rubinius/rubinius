require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.truncate" do  
  before :each do
    @name = "test.txt"
    @file  = File.open(@name, 'w')
    File.open(@name,"w") { |f| f.write("1234567890") }
  end

  after :each do
    @file.close
    File.delete(@name) if File.exist?(@name)
    @name = nil
  end  
 
  it "truncate the a file" do 
    File.open(@name, "w") { |f| f.puts "123456789" } 
    platform_is :mswin do
      File.size(@name).should == 11
    end
    
    platform_is_not :mswin do
      File.size(@name).should == 10
    end
    File.truncate(@name, 5)
    File.size(@name).should == 5
    File.open(@name, "r") do |f|
      f.read(99).should == "12345"
      f.eof?.should == true
    end
  end  

  it "truncate to 0 a file" do
    File.truncate(@name, 0).should == 0    
    IO.read(@name).should == ""
  end
 
  it "truncate to 5 a file"  do
    File.size(@name).should == 10
    File.truncate(@name, 5) 
    File.size(@name).should == 5
    IO.read(@name).should == "12345"
  end

  it "truncate to a larger size than the original file" do
    File.truncate(@name, 12) 
    File.size(@name).should == 12
    IO.read(@name).should == "1234567890\000\000"
  end

  it "truncate to a the same size the original file" do
    File.truncate(@name, File.size(@name))    
    File.size(@name).should == 10
    IO.read(@name).should == "1234567890"
  end

  it "raises Errno::ENOENT if the file does not exist" do
    not_existing_file = "file-does-not-exist-for-sure.txt"

    # make sure it doesn't exist for real
    File.delete(not_existing_file) if File.exist?(not_existing_file)

    begin
      lambda { File.truncate(not_existing_file, 5) }.should raise_error(Errno::ENOENT)
    ensure
      File.delete(not_existing_file) if File.exist?(not_existing_file)
    end
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments" do
    lambda { File.truncate(@name)      }.should raise_error(ArgumentError)
    lambda { File.truncate(@name, -1)  }.should raise_error(Errno::EINVAL) # May fail
    lambda { File.truncate(@name, nil) }.should raise_error(TypeError)
  end
end
