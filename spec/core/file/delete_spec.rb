require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.delete" do     
  before :each do     
    @file1 = 'test.txt'
    @file2 = 'test2.txt'
    File.delete(@file1) if File.exists?(@file1)
    File.delete(@file2) if File.exists?(@file2)

    File.open(@file1, "w"){} # Touch
    File.open(@file2, "w"){} # Touch
  end 

  it "delete a file " do
    File.delete.should == 0
    File.delete(@file1).should == 1    
    File.exists?(@file1).should == false
  end

  it "should delete multiple files" do
    File.delete(@file1, @file2).should == 2    
    File.exists?(@file1).should == false
    File.exists?(@file2).should == false
  end

  it "raise an exception if the arguments are wrong type or are the incorect number of arguments " do
    should_raise(TypeError){ File.delete(1) }
  end
  
  it "raise an error when the target file no exists " do 
    should_raise(Errno::ENOENT){ File.delete('bogus') }
  end

  it "delete a file using the alias unlink"  do
    File.unlink(@file1) 
    File.exists?(@file1).should == false
  end  
  
  it "deletes the named files" do
    File.delete(@file1)
    should_raise(Errno::ENOENT){File.open(@file1,"r")}
  end
    
  it "return the number of names passed as arguments (0 and 1 argument)" do
    File.delete.should == 0
    File.delete(@file1).should == 1
  end

  it "return the number of names passed as arguments (multiple arguments)" do
    File.delete(@file1, @file2).should == 2
  end
 
  specify "delete should delete a file and return the number of names passed as arguments" do
    begin
      File.delete(@file1) if File.exists?(@file1)
      File.exists?(@file1).should == false
      File.new(@file1,'w').close
      File.exists?(@file1).should == true
      File.delete(@file1).should == 1
      File.exists?(@file1).should == false
    ensure
      File.delete(@file1) rescue nil
    end
  end  
  
  it "should coerce a given parameter into a string if possible" do
    class Coercable; def to_str; "test.txt"; end; end
    File.delete(Coercable.new).should == 1
  end
  
  after :each do
    File.delete(@file1) if File.exists?(@file1)
    File.delete(@file2) if File.exists?(@file2)

    @file1 = nil
    @file2 = nil
  end
end
