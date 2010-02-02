describe :dir_delete, :shared => true do
  before :all do
    DirSpecs.rmdir_dirs
  end

  after :all do
    DirSpecs.rmdir_dirs false
  end

  it "removes empty directories" do
    Dir.send(@method, DirSpecs.mock_rmdir("empty")).should == 0
  end

  ruby_version_is "1.9" do
    it "calls #to_path on non-String arguments" do
      DirSpecs.rmdir_dirs
      p = mock('path')
      p.should_receive(:to_path).and_return(DirSpecs.mock_rmdir("empty"))
      Dir.send(@method, p)
    end
  end

  ruby_version_is ""..."1.9" do
    it "calls #to_str on non-String arguments" do
      DirSpecs.rmdir_dirs
      p = mock('path')
      p.should_receive(:to_str).and_return(DirSpecs.mock_rmdir("empty"))
      Dir.send(@method, p)
    end
  end

  it "raises a SystemCallError when trying to remove a nonempty directory" do
    lambda do
      Dir.send @method, DirSpecs.mock_rmdir("nonempty")
    end.should raise_error(SystemCallError)
  end

  # this won't work on Windows, since chmod(0000) does not remove all permissions
  platform_is_not :windows do
    it "raises a SystemCallError if lacking adequate permissions to remove the directory" do
      File.chmod(0000, DirSpecs.mock_rmdir("noperm"))
      lambda do
        Dir.send @method, DirSpecs.mock_rmdir("noperm", "child")
      end.should raise_error(SystemCallError)
    end
  end
end
