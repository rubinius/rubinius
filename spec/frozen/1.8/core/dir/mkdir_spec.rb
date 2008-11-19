require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir.mkdir" do
  it "creates the named directory with the given permissions" do
    DirSpecs.clear_dirs

    begin
      File.exist?('nonexisting').should == false
      Dir.mkdir 'nonexisting'
      File.exist?('nonexisting').should == true
      platform_is_not :windows do
        Dir.mkdir 'default_perms'
        a = File.stat('default_perms').mode
        Dir.mkdir 'reduced', (a - 1)
        File.stat('reduced').mode.should_not == a
      end
      platform_is :windows do
        Dir.mkdir 'default_perms', 0666
        a = File.stat('default_perms').mode
        Dir.mkdir 'reduced', 0444
        File.stat('reduced').mode.should_not == a
      end

      Dir.mkdir('always_returns_0').should == 0
      platform_is_not(:windows) do
        File.chmod(0777, "nonexisting","default_perms","reduced","always_returns_0")
      end
      platform_is_not(:windows) do
        File.chmod(0644, "nonexisting","default_perms","reduced","always_returns_0")
      end
    ensure
      DirSpecs.clear_dirs
    end
  end

  it "raises a SystemCallError when lacking adequate permissions in the parent dir" do
    # In case something happened it it didn't get cleaned up.
      FileUtils.rm_rf 'noperms' if File.directory? 'noperms'

    Dir.mkdir 'noperms', 0000

    lambda { Dir.mkdir 'noperms/subdir' }.should raise_error(SystemCallError)

    system 'chmod 0777 noperms'
    platform_is_not :windows do
      File.chmod 0777, "noperms"
    end
    platform_is :windows do
      File.chmod 0666, "noperms"
    end
    Dir.rmdir 'noperms'
  end

  it "raises a SystemCallError if any of the directories in the path before the last does not exist" do
    lambda { Dir.mkdir "#{DirSpecs.nonexistent}/subdir" }.should raise_error(SystemCallError)
  end
end
