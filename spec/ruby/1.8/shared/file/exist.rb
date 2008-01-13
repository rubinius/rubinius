shared :file_exist do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    it "return true if the file exist" do
      klass.send(cmd, __FILE__).should == true
      klass.send(cmd, 'a_fake_file').should == false
    end

    it "return true if the file exist using the alias exists?" do 
      klass.send(cmd, __FILE__).should == true
      klass.send(cmd, 'a_fake_file').should == false
    end

    it "raises an ArgumentError if not passed one argument" do
      lambda { klass.send(cmd) }.should raise_error(ArgumentError)
      lambda { klass.send(cmd, __FILE__, __FILE__) }.should raise_error(ArgumentError)
    end
    
    it "raises a TypeError if not passed a String type" do
      lambda { klass.send(cmd, nil) }.should raise_error(TypeError)
    end 
  end
end
