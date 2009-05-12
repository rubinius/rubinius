require File.dirname(__FILE__) + '/../../spec_helper'

describe "File#path" do
  before :each do
    @file1 = "testfile"
    @file2 = tmp("../tmp/xxx")
  end

  after :each do
    File.delete(@file1) if File.exist?(@file1)
    File.delete(@file2) if File.exist?(@file2)
  end

  it "returns the pathname used to create file as a string" do
    File.open(@file1,'w'){|file| file.path.should == "testfile"}
    File.open(@file2, 'w'){|file| file.path.should == tmp("../tmp/xxx")}
  end
end

describe "File.path" do
  before :each do
    @file1 = tmp("../tmp/xxx")
  end

  ruby_version_is "1.9.1" do
    it "returns the full path for the given file" do
      File.path(@file1).should == tmp("../tmp/xxx")
    end
  end
end