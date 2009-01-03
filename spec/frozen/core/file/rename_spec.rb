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

  platform_is_not :windows do
    it "renames a file " do
      File.exists?(@old).should == true
      File.exists?(@new).should == false
      File.rename(@old, @new)
      File.exists?(@old).should == false
      File.exists?(@new).should == true
    end

    it "raises an Errno::ENOENT if the source does not exist" do
      File.delete(@old)
      lambda { File.rename(@old, @new) }.should raise_error(Errno::ENOENT)
    end

    it "raises an ArgumentError if not passed two arguments" do
      lambda { File.rename        }.should raise_error(ArgumentError)
      lambda { File.rename(@file) }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed String types" do
      lambda { File.rename(1, 2)  }.should raise_error(TypeError)
    end
  end
end
