shared :dir_delete do |cmd|
  describe "Dir.#{cmd}" do
    it "removes empty directories" do
      %w|rmdir delete unlink|.each {|cmd|
        Dir.mkdir 'empty_subdir'
        Dir.send(cmd, 'empty_subdir').should == 0
      }
    end

    it "raises a SystemCallError when trying to remove a nonempty directory" do
      %w|rmdir delete unlink|.each {|cmd|
        lambda { Dir.send cmd, 'subdir_one' }.should raise_error(SystemCallError)
      }
    end

    it "raises a SystemCallError if lacking adequate permissions to remove the directory" do
      %w|rmdir delete unlink|.each {|cmd|
        system "mkdir -p noperm_#{cmd}/child"
        system "chmod 0000 noperm_#{cmd}"

        lambda { Dir.send cmd, "noperm_#{cmd}/child" }.should raise_error(SystemCallError)

        system "chmod 0777 noperm_#{cmd}"
        Dir.rmdir "noperm_#{cmd}/child"
        Dir.rmdir "noperm_#{cmd}"
      }
    end
  end
end
