shared :dir_open do |cmd|
  describe "Dir.#{cmd}" do
    it "returns a Dir instance representing the specified directory" do
      dir = Dir.send(cmd, DirSpecs.mock_dir)
      dir.class.should == Dir
      dir.close
    end
  
    it "raises a SystemCallError if the directory does not exist" do
      lambda {  Dir.send cmd, DirSpecs.nonexistent }.should raise_error(SystemCallError)
    end
  
    it "takes a block which yields the Dir instance and closes it after" do
      closed_dir = Dir.send(cmd, DirSpecs.mock_dir) { |dir| dir }
      lambda { closed_dir.close }.should raise_error(IOError, "closed directory")
    end
  
    it "returns the value of the block if a block is passed" do
      Dir.open(DirSpecs.mock_dir) {|dir| dir.class }.should == Dir
    end
  end
end
