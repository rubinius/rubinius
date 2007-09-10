require File.dirname(__FILE__) + '/../../spec_helper'

describe "File#chmod" do
  before(:each) do
    @filename = '/tmp/i_exist'
    @file = File.open(@filename, 'w')
  end
  
  it "should modify the permission bits of the files specified" do
    @file.chmod(0755).should == 0
    File.stat(@filename).mode.should == 33261
  end
  
  after(:each) do
    @file.close
    File.delete(@filename) if File.exist?(@filename)
  end
end

describe "File.chmod" do
  before(:each) do
    @file = '/tmp/i_exist'
    File.open(@file, 'w') {}
    @count = File.chmod(0755, @file)
  end
  
  it "should return the number of files modified" do
    @count.should == 1
  end
  
  it "should modify the permission bits of the files specified" do
    File.stat(@file).mode.should == 33261
  end
  
  after(:each) do
    File.delete(@file) if File.exist?(@file)
  end  
end
