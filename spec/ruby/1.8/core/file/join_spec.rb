require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.join" do

  it "does nothing to empty strings" do
    File.join("").should == ""
  end

  it "joins parts using File::SEPARATOR" do
    File.join('usr', 'bin').should == "usr/bin"
  end

  platform_is :mswin do
    it "joins parts using File::ALT_SEPARATOR on mswin" do
      File.join("C:\\", 'windows').should == "C:\\windows"
      File.join("\\\\", "usr").should = "\\\\usr"
    end
  end

  it "handles leading parts edge cases" do
    File.join("/bin")     .should == "/bin"
    File.join("", "bin")  .should == "/bin"
    File.join("/", "bin") .should == "/bin"
    File.join("/", "/bin").should == "/bin"
  end

  it "handles trailing parts edge cases" do
    File.join("bin", "")  .should == "bin/"
    File.join("bin/")     .should == "bin/"
    File.join("bin/", "") .should == "bin/"
    File.join("bin", "/") .should == "bin/"
    File.join("bin/", "/").should == "bin/"
  end

  it "handles middle parts edge cases" do
    File.join("usr",   "", "bin") .should == "usr/bin"
    File.join("usr/",  "", "bin") .should == "usr/bin"
    File.join("usr",   "", "/bin").should == "usr/bin"
    File.join("usr/",  "", "/bin").should == "usr/bin"
  end

  it "doesn't remove File::SEPARATOR from the middle of arguments" do
    path = File.join "file://usr", "bin"
    path.should == "file://usr/bin"
  end

  it "raises a TypeError exception when args are nil" do
    lambda { File.join nil }.should raise_error(TypeError)
  end

  it "joins parts using File::SEPARATOR" do
    File.join("smalltalk","ruby","rubinius").should == "smalltalk/ruby/rubinius"
    File.join.should == ""

    # arguments must respond to to_str
    lambda { File.join(mock('x')) }.should raise_error(TypeError)
  end

end

