require File.dirname(__FILE__) + '/../spec_helper'

# specs for __FILE__

describe "The __FILE__ constant" do
  it "equals the current filename" do
    File.basename(__FILE__).should == "file_spec.rb"
  end

  it "equals (eval) inside an eval" do
    eval("__FILE__").should == "(eval)"
  end
  
  it "equals a relative path when required using a relative path" do
    base_path = File.dirname(File.dirname(fixture(__FILE__, "file.rb")))
    path = "fixtures/file.rb"
    Dir.chdir(base_path) do
      require path
      ScratchPad.recorded.should == File.join(".",path)
    end
  end
  
  it "equals the full path when required using a full path" do
    path = fixture(__FILE__, "file.rb")
    require path
    ScratchPad.recorded.should == path
  end
end


describe "The __FILE__ constant" do
  before(:each) do
    path = fixture(__FILE__,"file.rb")
    #puts "@@@@ Path is #{path} for fixture(#{__FILE__},'file.rb')"
    $:.unshift File.dirname(path)
  end
  after(:each) do
    $:.shift
  end
  
  it "equals the full path to the file when required" do
    require 'file.rb'
    ScratchPad.recorded.should == fixture(__FILE__, 'file.rb')
  end
  
end
