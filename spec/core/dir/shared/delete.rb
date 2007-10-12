@dir_delete = shared "Dir.delete" do |cmd|
  describe "Dir.#{cmd}" do
    specify "removes empty directories" do
      %w|rmdir delete unlink|.each {|cmd|
        Dir.mkdir 'empty_subdir'
        Dir.send(cmd, 'empty_subdir').should == 0
      }
    end

    it "raises SystemCallError when trying to remove a nonempty directory" do
      %w|rmdir delete unlink|.each {|cmd|
        should_raise(SystemCallError) { Dir.send cmd, 'subdir_one' }
      }
    end

    it "raises SystemCallError if lacking adequate permissions to remove the directory" do
      %w|rmdir delete unlink|.each {|cmd|
        system "mkdir -p noperm_#{cmd}/child"
        system "chmod 0000 noperm_#{cmd}"

        should_raise(SystemCallError) { Dir.send cmd, "noperm_#{cmd}/child" }

        system "chmod 0777 noperm_#{cmd}"
        Dir.rmdir "noperm_#{cmd}/child"
        Dir.rmdir "noperm_#{cmd}"
      }
    end
  end
end
