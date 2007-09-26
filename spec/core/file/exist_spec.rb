require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.exist?" do 
  before :each do
    @file = 'temp.txt'
    File.open(@file, "w"){}
  end 

  after :each do
    File.delete(@file) if File.exist?(@file)
    @file = nil
  end  
  
  it "return true if the file exist" do
    File.exist?(@file).should == true
    File.exist?('a_fake_file').should == false
  end

  it "return true if the file exist using the alias exists?" do 
    File.exists?(@file).should == true
    File.exists?('a_fake_file').should == false
  end

  it "raise an exception if the argumnent is not from the correct type or are missing" do
    should_raise(ArgumentError){ File.exist? }
    should_raise(ArgumentError){ File.exist?(@file, @file) }
    should_raise(TypeError){ File.exist?(nil) }
  end 
end
