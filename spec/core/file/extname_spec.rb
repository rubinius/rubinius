require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.extname" do
  it "returns the extension (the portion of file name in path after the period)." do 
    File.extname("foo.rb").should == ".rb"
    File.extname("/foo/bar.rb").should == ".rb"
    File.extname("/foo.rb/bar.c").should == ".c"
    File.extname("bar").should == ""
    File.extname(".bashrc").should == ""
    File.extname("/foo.bar/baz").should == ""
    File.extname(".app.conf").should == ".conf"
  end

  it "returns the extension (the portion of file name in path after the period).(edge cases)" do 
    File.extname("").should ==  ""
    File.extname(".").should ==  ""
    File.extname("/").should ==  ""
    File.extname("/.").should ==  ""
    File.extname("..").should ==  ""
    File.extname("...").should ==  ""
    File.extname("....").should ==  ""
    File.extname(".foo.").should ==  ""
    File.extname("foo.").should ==  ""
  end
  
  it "returns only the last extension of a file with several dots" do
    File.extname("a.b.c.d.e").should == ".e"
  end

  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(TypeError){ File.extname(nil) }
    should_raise(TypeError){ File.extname(0) }
    should_raise(TypeError){ File.extname(true) }
    should_raise(TypeError){ File.extname(false) }
    should_raise(ArgumentError){ File.extname("foo.bar", "foo.baz") }
  end  
end
