shared :filetest_exist do |cmd|
  describe "FileTest.#{cmd}" do
    it "return true if the file exist" do
      FileTest.exist?(__FILE__).should == true
      FileTest.exist?('a_fake_file').should == false
    end
  
    it "return true if the file exist using the alias exists?" do 
      FileTest.exists?(__FILE__).should == true
      FileTest.exists?('a_fake_file').should == false
    end
  
    it "raises an ArgumentError if not passed one argument" do
      lambda { FileTest.exist? }.should raise_error(ArgumentError)
      lambda { FileTest.exist?(__FILE__, __FILE__) }.should raise_error(ArgumentError)
    end
    
    it "raises a TypeError if not passed a String type" do
      lambda { FileTest.exist?(nil) }.should raise_error(TypeError)
      lambda { FileTest.exist?(1)   }.should raise_error(TypeError)
    end 
  end
end
