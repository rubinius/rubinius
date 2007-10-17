require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.join" do   
  before(:each) do
    platform :mswin do
      @root = "C:\\"
    end
    
    platform :not, :mswin do
      @root = "/"
    end
    @dirs = ['usr', 'local', 'bin']
  end 

  after(:each) do
    @root = nil
    @dirs = nil
  end

  platform :mswin do
    it "returns a new string formed by joining the strings using File::SEPARATOR (windows)" do 
      File.join(*@dirs).should == "usr/local/bin"
      File.join(@root, *@dirs).should == "C:\\usr/local/bin"
    end

    # FIXME: needs edge cases from unix
    it "returns a new string formed by joining the strings using File::SEPARATOR (edge cases on windows) " do 
      File.join("").should = ""
      File.join("", "foo").should == "/foo"
      File.join("usr", "", "local", "", "bin").should == "usr/local/bin"
      File.join("\\\\", "usr", "local").should = "\\\\usr/local"
    end
  end
  
  platform :not, :mswin do 
    it "returns a new string formed by joining the strings using File::SEPARATOR (unix)" do 
      File.join(*@dirs).should == "usr/local/bin"
      File.join(@root, *@dirs).should == "/usr/local/bin"
    end
    
    it "returns a new string formed by joining the strings using File::SEPARATOR (edge cases on unix)" do 
      File.join("").should == ""
      File.join("", "foo").should == "/foo"
      File.join("usr", "", "local", "", "bin").should == "usr/local/bin"
      File.join("","usr", "", "local", "", "bin").should == "/usr/local/bin"
      File.join("usr", "", "local", "", "bin", "").should == "usr/local/bin/"
      File.join("", "usr", "local", "", "bin", "").should == "/usr/local/bin/"
      File.join("", "usr", "local", "", "bin", "/").should == "/usr/local/bin/"
      File.join("/", "usr", "local", "", "bin", "").should == "/usr/local/bin/"
      File.join("", "usr", "local/", "", "bin", "").should == "/usr/local/bin/"
      File.join("", "usr", "local/", "", "bin/", "").should == "/usr/local/bin/"
    end
  end

  it "returns a new string formed by joining the strings using File::SEPARATOR (any plataform)" do 
    [ %w( a b c d ), %w( a ), %w( ), %w( a b .. c ) ].each do |a|
      a.join(File::SEPARATOR).should == File.join(*a)
    end
  end
  
  it "raise a TypeError exception when args are nil" do
    should_raise(TypeError){ File.join(nil, nil) }
  end
end

describe "File.join" do
  it "should return a new string formed by joining the strings using File::SEPARATOR" do
    File.join("smalltalk","ruby","rubinius").should == "smalltalk/ruby/rubinius"
    File.join.should == ""

    # arguments must respond to to_str
    should_raise(TypeError) do
      File.join(Object.new)
    end
  end
end
