describe :dir_delete, :shared => true do
  it "removes empty directories" do
    Dir.mkdir 'empty_subdir'
    Dir.send(@method, 'empty_subdir').should == 0
  end

  it "raises a SystemCallError when trying to remove a nonempty directory" do
    lambda { Dir.send @method, 'subdir_one' }.should raise_error(SystemCallError)
  end

  it "raises a SystemCallError if lacking adequate permissions to remove the directory" do
    system "mkdir -p noperm_#{@method}/child"
    system "chmod 0000 noperm_#{@method}"

    lambda { Dir.send @method, "noperm_#{@method}/child" }.should raise_error(SystemCallError)

    system "chmod 0777 noperm_#{@method}"
    Dir.rmdir "noperm_#{@method}/child"
    Dir.rmdir "noperm_#{@method}"
  end
end
