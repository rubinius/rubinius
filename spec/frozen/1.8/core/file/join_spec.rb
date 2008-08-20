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

  it "handles recursive arrays" do
    parts = []
    parts << parts
    File.join(parts).should == '[...]'

    parts = ["one", "two"]
    parts << parts
    File.join(parts).should == 'one/two/one/two/[...]'

    parts << "three"
    parts << "four"
    File.join(parts).should == 'one/two/one/two/[...]/three/four/three/four'

    parts = [["one", "two"], ["three", "four"]]
    parts << parts
    File.join(parts).should == 'one/two/three/four/one/two/three/four/[...]'

    a = ['a']
    a << a
    File.join(a).should == 'a/a/[...]'
    File.join([a]).should == 'a/a/[...]'

    a = ['a']
    b = ['b']
    a << b
    b << a
    File.join(a).should == "a/b/[...]"

    a = []
    b = []
    a << b
    b << a
    File.join(a).should == '[...]'

    a = ['a']
    a << a
    File.join(  a  ).should      == "a/a/[...]"
    File.join( [a] ).should      == "a/a/[...]"
    File.join([[a]]).should      == "a/a/[...]"
    File.join(  a, 'b'  ).should == "a/a/[...]/b"
    File.join( [a, 'b'] ).should == "a/a/[...]/b"
    File.join([[a, 'b']]).should == "a/a/[...]/b"
    File.join([[a], 'b']).should == "a/a/[...]/b"
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
