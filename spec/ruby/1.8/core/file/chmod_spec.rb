require File.dirname(__FILE__) + '/../../spec_helper'

describe "File#chmod" do
  before :each do
    @filename = File.dirname(__FILE__) + '/fixtures/i_exist'
    @file = File.open(@filename, 'w')
  end
    
  after :each do
    @file.close
    File.delete(@filename) if File.exist?(@filename)
  end
  
  it "returns 0 if successful" do
    @file.chmod(0755).should == 0
  end 
    
  platform_is_not :mswin do
    it "modifies the permission bits of the files specified" do
      @file.chmod(0755)
      File.stat(@filename).mode.should == 33261
    end
  end
end

describe "File.chmod" do
  before :each do
    @file = File.dirname(__FILE__) + '/fixtures/i_exist'
    File.open(@file, 'w') {}
    @count = File.chmod(0755, @file)
  end
  
  after :each do
    File.delete(@file) if File.exist?(@file)
  end  
  
  it "returns the number of files modified" do
    @count.should == 1
  end
  
  platform_is_not :mswin do
    it "modifies the permission bits of the files specified" do
      File.stat(@file).mode.should == 33261
    end
  end
end