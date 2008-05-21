require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.split" do
  before :each do
    @path_unix             = "/foo/bar/baz.rb"
    @path_windows_backward = "C:\\foo\\bar\\baz.rb"
    @path_windows_forward  = "C:/foo/bar/baz.rb"
  end

  it "splits the given string into a directory and a file component and returns them in a 2 element array" do
    File.split("/rubinius/better/than/ruby").should == ["/rubinius/better/than", "ruby"]
  end

  it "splits the given string into a directory and a file component and returns them in a two-element array. (unix)" do
    File.split(@path_unix).should == ["/foo/bar","baz.rb"]
  end

  it "splits the given string into a directory and a file component and returns them in a two-element array. (edge cases)" do
    File.split("").should == [".", ""]
    File.split("//foo////").should == ["/", "foo"]
  end

  it "splits the given string into a directory and a file component and returns them in a two-element array. (windows)" do
    compliant_on :ruby, :rubinius do
      File.split(@path_windows_backward).should ==  [".", "C:\\foo\\bar\\baz.rb"]
    end

    # Actually, MRI on windows behaves like this too, and that seems
    # to be most proper behavior:
    compliant_on :jruby do
      File.split(@path_windows_backward).should ==  ["C:\\foo\\bar", "baz.rb"]
    end

    # Note: MRI on Cygwin exhibits third type of behavior,
    # different from *both* variants above...
  end

  it "splits the given string into a directory and a file component and returns them in a two-element array. (forward slash)" do
    File.split(@path_windows_forward).should == ["C:/foo/bar", "baz.rb"]
  end

  it "raises an ArgumentError when not passed a single argument" do
    lambda { File.split }.should raise_error(ArgumentError)
    lambda { File.split('string', 'another string') }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if the argument is not a String type" do
    lambda { File.split(1) }.should raise_error(TypeError)
  end

  it "coerces the argument with to_str if it is not a String type" do
    class C; def to_str; "/rubinius/better/than/ruby"; end; end
    File.split(C.new).should == ["/rubinius/better/than", "ruby"]
  end
end
