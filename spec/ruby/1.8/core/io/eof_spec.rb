require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#eof?" do
  before :each do
    @fname = "test.txt"
    @contents = "1"
    File.open(@fname, "w") { |f| f.write(@contents) }
  end
  
  after :each do
    File.delete(@fname) if File.exists?(@fname)
  end
  
  it "is at EOF after reading" do
    open @fname do |f|
      f.read
      f.eof?.should == true
    end
  end
  
  it "is not at EOF on open" do
    open @fname do |f|
      f.eof?.should == false
    end
  end
end

