shared :dir_closed do |cmd|
  describe "Dir##{cmd}" do
    it "raises an IOError when called on a closed Dir instance" do
      lambda {
        dir = Dir.open DirSpecs.mock_dir
        dir.close
        dir.send(cmd) {}
      }.should raise_error(IOError)
    end
  end
end
