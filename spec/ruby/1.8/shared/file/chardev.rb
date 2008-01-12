shared :file_chardev do |cmd, klass|
  describe "#{klass}.#{cmd}" do
    it "returns true/false depending if the named file is a char device" do
      klass.send(cmd, "/tmp").should == false
    end
  end 
end
