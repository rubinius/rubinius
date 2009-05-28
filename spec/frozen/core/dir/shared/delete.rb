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

  it "raises a SystemCallError when trying to remove a nonempty directory" do
    lambda do
      Dir.send @method, DirSpecs.mock_rmdir("nonempty")
    end.should raise_error(SystemCallError)
  end

  it "raises a SystemCallError if lacking adequate permissions to remove the directory" do
    File.chmod(0000, DirSpecs.mock_rmdir("noperm"))
    lambda do
      Dir.send @method, DirSpecs.mock_rmdir("noperm", "child")
    end.should raise_error(SystemCallError)
  end
end
