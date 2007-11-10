require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.rename" do
  before :each do   
    @old = "test.txt"
    @new = "test.new"
    File.delete(@old) if File.exist?(@old)
    File.delete(@new) if File.exist?(@new)
    File.open(@old,"w+") {|f| f.puts "hello" }
  end 

  after :each do
    File.delete(@old) if File.exist?(@old)
    File.delete(@new) if File.exist?(@new)
  end

  platform :not, :mswin do
    it "renames a file " do
      File.exists?(@old).should == true
      File.exists?(@new).should == false
      File.rename(@old, @new)
      File.exists?(@old).should == false
      File.exists?(@new).should == true
    end

    it "raises an exception if the source does not exist" do
      File.delete(@old)
      should_raise(Errno::ENOENT){ File.rename(@old, @new) }
    end

    it "raises an exception if the arguments are wrong type or are the incorect number of arguments" do
      should_raise(ArgumentError){ File.rename }
      should_raise(ArgumentError){ File.rename(@file) }
      should_raise(TypeError){ File.rename(1, 2) }
    end
  end
end
