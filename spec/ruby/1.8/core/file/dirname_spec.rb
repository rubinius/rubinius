require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.dirname" do
  it "dirname should return all the components of filename except the last one" do
    File.dirname('/home/jason').should == '/home'
    File.dirname('/home/jason/poot.txt').should == '/home/jason'
    File.dirname('poot.txt').should == '.'
    File.dirname('/holy///schnikies//w00t.bin').should == '/holy///schnikies'
    File.dirname('').should == '.'
    File.dirname('/').should == '/'
    File.dirname('/////').should == '/'
  end

  it "return a String" do
    File.dirname("foo").class.should == String
  end

  it "not modify its argument" do
    x = "/usr/bin"
    File.dirname(x)
    x.should == "/usr/bin"
  end

  it "return the return all the components of filename except the last one (unix format)" do
    File.dirname("foo").should =="."
    File.dirname("/foo").should =="/"
    File.dirname("/foo/bar").should =="/foo"
    File.dirname("/foo/bar.txt").should =="/foo"
    File.dirname("/foo/bar/baz").should =="/foo/bar"
  end

  platform_is_not :windows do
    it "return all the components of filename except the last one (edge cases)" do
      File.dirname("").should == "."
      File.dirname(".").should == "."
      File.dirname("./").should == "."
      File.dirname("./b/./").should == "./b"
      File.dirname("..").should == "."
      File.dirname("../").should == "."
      File.dirname("/").should == "/"
      File.dirname("/.").should == "/"
      File.dirname("/foo/").should == "/"
      File.dirname("//foo//").should == "/"
      File.dirname("/foo/.").should == "/foo"
      File.dirname("/foo/./").should == "/foo"
      File.dirname("/foo/../.").should == "/foo/.."
      File.dirname("foo/../").should == "foo"
    end
  end

  platform_is :windows do
    it "return all the components of filename except the last one (edge cases)" do
      File.dirname("//foo").should == "/"
    end
  end

  it "raises a TypeError if not passed a String type" do
    lambda { File.dirname(nil)   }.should raise_error(TypeError)
    lambda { File.dirname(0)     }.should raise_error(TypeError)
    lambda { File.dirname(true)  }.should raise_error(TypeError)
    lambda { File.dirname(false) }.should raise_error(TypeError)
  end

  # Windows specific tests
  platform_is :windows do
    it "return the return all the components of filename except the last one (Windows format)" do
      File.dirname("C:\\foo\\bar\\baz.txt").should =="C:\\foo\\bar"
      File.dirname("C:\\foo\\bar").should =="C:\\foo"
      File.dirname("C:\\foo\\bar\\").should == "C:\\foo"
      File.dirname("C:\\foo").should == "C:\\"
      File.dirname("C:\\").should =="C:\\"
    end

    it "return the return all the components of filename except the last one (windows unc)" do
      File.dirname("\\\\foo\\bar\\baz.txt").should == "\\\\foo\\bar"
      File.dirname("\\\\foo\\bar\\baz").should == "\\\\foo\\bar"
      File.dirname("\\\\foo").should =="\\\\foo"
      File.dirname("\\\\foo\\bar").should =="\\\\foo\\bar"
    end

    it "return the return all the components of filename except the last one (forward_slash)" do
      File.dirname("C:/").should == "C:/"
      File.dirname("C:/foo").should == "C:/"
      File.dirname("C:/foo/bar").should == "C:/foo"
      File.dirname("C:/foo/bar/").should == "C:/foo"
      File.dirname("C:/foo/bar//").should == "C:/foo"
    end
  end
end
