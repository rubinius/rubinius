@filetest_exist = shared "FileTest.exist?" do |cmd|
  describe "FileTest.#{cmd}" do
    it "return true if the file exist" do
      FileTest.exist?(__FILE__).should == true
      FileTest.exist?('a_fake_file').should == false
    end
  
    it "return true if the file exist using the alias exists?" do 
      FileTest.exists?(__FILE__).should == true
      FileTest.exists?('a_fake_file').should == false
    end
  
    it "raise an exception if the argumnent is not from the correct type or are missing" do
      should_raise(ArgumentError){ FileTest.exist? }
      should_raise(ArgumentError){ FileTest.exist?(__FILE__, __FILE__) }
      should_raise(TypeError){ FileTest.exist?(nil) }
    end 
  end
end
