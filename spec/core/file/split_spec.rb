require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.split" do 
  before :each do
    @path_unix             = "/foo/bar/baz.rb"
    @path_windows_backward = "C:\\foo\\bar\\baz.rb"
    @path_windows_forward  = "C:/foo/bar/baz.rb"
  end 
  
  it "should split the given string into a directory and a file component and returns them in a 2 element array" do
    File.split("/rubinius/better/than/ruby").should == ["/rubinius/better/than", "ruby"]
  end

  it "should split the given string into a directory and a file component and returns them in a two-element array. (unix)" do
    File.split(@path_unix).should == ["/foo/bar","baz.rb"]
  end

  it "should split the given string into a directory and a file component and returns them in a two-element array. (edge cases)" do 
    File.split("").should == [".", ""]
    File.split("//foo////").should == ["/", "foo"]  
  end

  it "should split the given string into a directory and a file component and returns them in a two-element array.(windows)" do
    File.split(@path_windows_backward).should ==  [".", "C:\\foo\\bar\\baz.rb"]
  end   
   
  it "should split the given string into a directory and a file component and returns them in a two-element array.(forward slash)" do
    File.split(@path_windows_forward).should == ["C:/foo/bar", "baz.rb"] 
  end
  
  it "should raise an exception if the number of arguments are incorrect or of incorrect type" do
    should_raise(ArgumentError){ File.split }
    should_raise(ArgumentError){ File.split('string', 'another string') }
    should_raise(TypeError){ File.split(1) }
  end
    
  it "should coerce argument to a string if a non string type is given" do
    class C; def to_str; "/rubinius/better/than/ruby"; end; end
    File.split(C.new).should == ["/rubinius/better/than", "ruby"]
  end
end
