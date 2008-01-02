shared :dir_open do |cmd|
  describe "Dir.#{cmd}" do
    it "returns a Dir instance representing the specified directory" do
      dir = Dir.send(cmd, DirSpecs.mock_dir)
      dir.class.should == Dir
      dir.close
    end
  
    it "raises SystemCallError if the directory does not exist" do
      lambda {  Dir.send cmd, DirSpecs.nonexistent }.should raise_error(SystemCallError)
    end
  
    it "takes a block which yields the Dir instance and closes it after" do
      # This is a bit convoluted but we are trying to ensure the file gets closed.
      # To do that, we peek to see what the next FD number is and then probe that
      # to see whether it has been closed.
      peek = IO.sysopen DirSpecs.mock_dir
      File.for_fd(peek).close

      Dir.send(cmd, DirSpecs.mock_dir) { }
      lambda { File.for_fd peek }.should raise_error(SystemCallError)
    end
  
    it "returns the value of the block if a block is passed" do
      Dir.open(DirSpecs.mock_dir) {|dir| dir.class}.should == Dir
    end
  end
end
