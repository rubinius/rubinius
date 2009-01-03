describe :dir_delete, :shared => true do
  it "removes empty directories" do
    Dir.mkdir 'empty_subdir'
    Dir.send(@method, 'empty_subdir').should == 0
  end

  it "raises a SystemCallError when trying to remove a nonempty directory" do
    lambda { Dir.send @method, 'subdir_one' }.should raise_error(SystemCallError)
  end

  it "raises a SystemCallError if lacking adequate permissions to remove the directory" do
    FileUtils.mkdir_p("noperm_#{@method}/child")
    File.chmod(0000, "noperm_#{@method}")

    lambda { Dir.send @method, "noperm_#{@method}/child" }.should raise_error(SystemCallError)

    platform_is_not(:windows) do
    system "chmod 0777 noperm_#{@method}"
    end
    platform_is(:windows) do
      File.chmod(0666, "noperm_#{@method}")
    end
    Dir.rmdir "noperm_#{@method}/child"
    Dir.rmdir "noperm_#{@method}"
  end
end
