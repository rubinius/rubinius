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

    it "may take a block which is yielded to with the Dir instance" do
      Dir.send(cmd, DirSpecs.mock_dir) {|dir| dir.class.should == Dir }
    end

    it "returns the value of the block if a block is given" do
      Dir.open(DirSpecs.mock_dir) {|dir| :value }.should == :value
    end

    it "closes the Dir instance when the block exits if given a block" do
      closed_dir = Dir.send(cmd, DirSpecs.mock_dir) { |dir| dir }
      lambda { closed_dir.close }.should raise_error(IOError, "closed directory")
    end

    it "closes the Dir instance when the block exits the block even due to an exception" do
      @closed_dir = nil

      l = lambda { Dir.send(cmd, DirSpecs.mock_dir) { |dir| @closed_dir = dir; raise } }
      l.should raise_error

      lambda { @closed_dir.close }.should raise_error(IOError, "closed directory")
    end
  end
end
