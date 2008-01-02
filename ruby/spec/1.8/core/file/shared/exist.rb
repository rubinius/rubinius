shared :file_exist do |cmd|
  describe "File.#{cmd}" do
    it "return true if the file exist" do
      File.exist?(__FILE__).should == true
      File.exist?('a_fake_file').should == false
    end

    it "return true if the file exist using the alias exists?" do 
      File.exists?(__FILE__).should == true
      File.exists?('a_fake_file').should == false
    end

    it "raise an exception if the argumnent is not from the correct type or are missing" do
      lambda { File.exist? }.should raise_error(ArgumentError)
      lambda { File.exist?(__FILE__, __FILE__) }.should raise_error(ArgumentError)
      lambda { File.exist?(nil) }.should raise_error(TypeError)
    end 
  end
end
