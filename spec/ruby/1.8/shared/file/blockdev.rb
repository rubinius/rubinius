shared :file_blockdev do |cmd, klass|
  describe "#{klass}.#{cmd}" do
    it "returns true/false depending if the named file is a block device" do
      klass.send(cmd, "/tmp").should == false
    end
  end
end
