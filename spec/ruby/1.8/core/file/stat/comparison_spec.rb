require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#<=>" do
  
  it "is able to compare files by the same modification times" do
    f1 = File.new("/tmp/i_exist", "w")
    f2 = File.new("/tmp/i_exist_too", "w")

    (f1.stat <=> f2.stat).should == 0
    
    File.delete("/tmp/i_exist") if File.exist?("/tmp/i_exist")
    File.delete("/tmp/i_exist_too") if File.exist?("/tmp/i_exist_too")
  end

  it "is able to compare files by different modification times" do
    f1 = File.new("/tmp/i_exist", "w")
    f2 = File.new("/tmp/i_exist_too", "w")

    File.utime(Time.now, Time.now + 100, "/tmp/i_exist_too")
    (f1.stat <=> f2.stat).should == -1

    File.utime(Time.now, Time.now - 100, "/tmp/i_exist_too")
    (f1.stat <=> f2.stat).should == 1
    
    File.delete("/tmp/i_exist") if File.exist?("/tmp/i_exist")
    File.delete("/tmp/i_exist_too") if File.exist?("/tmp/i_exist_too")
  end

end
