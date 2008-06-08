require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#<=>" do
  
  it "is able to compare files by the same modification times" do
    f1 = File.new(tmp("i_exist"), "w")
    f2 = File.new(tmp("i_exist_too"), "w")

    (f1.stat <=> f2.stat).should == 0
    
    File.delete(tmp("i_exist")) if File.exist?(tmp("i_exist"))
    File.delete(tmp("i_exist_too")) if File.exist?(tmp("i_exist_too"))
  end

  it "is able to compare files by different modification times" do
    f1 = File.new(tmp("i_exist"), "w")
    f2 = File.new(tmp("i_exist_too"), "w")

    File.utime(Time.now, Time.now + 100, tmp("i_exist_too"))
    (f1.stat <=> f2.stat).should == -1

    File.utime(Time.now, Time.now - 100, tmp("i_exist_too"))
    (f1.stat <=> f2.stat).should == 1
    
    File.delete(tmp("i_exist")) if File.exist?(tmp("i_exist"))
    File.delete(tmp("i_exist_too")) if File.exist?(tmp("i_exist_too"))
  end
  
  it "should also include Comparable and thus == shows mtime equality between two File::Stat objects" do
    f1 = File.new(tmp("i_exist"), "w")
    f2 = File.new(tmp("i_exist_too"), "w")
    
    (f1.stat == f2.stat).should == true
    (f1.stat == f1.stat).should == true
    (f2.stat == f2.stat).should == true
    
    File.utime(Time.now, Time.now + 100, tmp("i_exist_too"))
    
    (f1.stat == f2.stat).should == false
    (f1.stat == f1.stat).should == true
    (f2.stat == f2.stat).should == true
  end

end
