require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.join" do
  it "returns an empty string when given no arguments" do
    File.join.should == ""
  end

  it "when given a single argument returns an equal string" do
    File.join("").should == ""
    File.join("usr").should == "usr"
  end

  it "joins parts using File::SEPARATOR" do
    File.join('usr', 'bin').should == "usr/bin"
  end

  it "supports any number of arguments" do
    File.join("a", "b", "c", "d").should == "a/b/c/d"
  end

  platform_is :windows do
    it "joins parts using File::ALT_SEPARATOR on windows" do
      File.join("C:\\", 'windows').should == "C:\\windows"
      File.join("\\\\", "usr").should == "\\\\usr"
    end
  end

  it "flattens nested arrays" do
    File.join(["a", "b", "c"]).should == "a/b/c"
    File.join(["a", ["b", ["c"]]]).should == "a/b/c"
  end
  
  it "inserts the separator in between empty strings and arrays" do  
    File.join("").should == ""
    File.join("", "").should == "/"
    File.join(["", ""]).should == "/"
    File.join("a", "").should == "a/"
    File.join("", "a").should == "/a"

    File.join([]).should == ""
    File.join([], []).should == "/"
    File.join([[], []]).should == "/"
    File.join("a", []).should == "a/"
    File.join([], "a").should == "/a"
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

  # TODO: Fix the version when this bug is fixed in MRI
  # We may need to add multiple versions because it will
  # be disjoint (ie 1.8.8.xxx and 1.9.2.xxx).
  ruby_bug "redmine #1418", "1.9" do
    it "returns '[...]' for the embedded array in a recursive array" do
      a = ["a"]
      a << a
      a << ["b", "c"]
      File.join(a).should == "a/[...]/b/c"
    end

    it "returns '[...]' for the embedded array in a nested recursive array" do
      a = [["a", "b"], ["c"]]
      a << a
      File.join(a).should == "a/b/c/[...]"
    end

    it "returns '[...]' for the embedded array in mutually recursive arrays" do
      a = ['a']
      b = ['b']
      a << b
      b << a
      File.join(a).should == "a/b/[...]"
    end
  end

  it "returns '[...]' for an empty recursive array" do
    parts = []
    parts << parts
    File.join(parts).should == '[...]'
  end

  it "doesn't remove File::SEPARATOR from the middle of arguments" do
    path = File.join "file://usr", "bin"
    path.should == "file://usr/bin"
  end

  it "raises a TypeError exception when args are nil" do
    lambda { File.join nil }.should raise_error(TypeError)
  end

  it "calls #to_str" do
    lambda { File.join(mock('x')) }.should raise_error(TypeError)

    bin = mock("bin")
    bin.should_receive(:to_str).exactly(:twice).and_return("bin")
    File.join(bin).should == "bin"
    File.join("usr", bin).should == "usr/bin"
  end

end
