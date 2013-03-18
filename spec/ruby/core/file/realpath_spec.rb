require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "File.realpath" do
    before :each do
      @real_dir = tmp('dir_realpath_real')
      @link_dir = tmp('dir_realpath_link')

      mkdir_p @real_dir
      File.symlink(@real_dir, @link_dir)

      @file = File.join(@real_dir, 'file')
      @link = File.join(@link_dir, 'link')

      touch @file
      File.symlink(@file, @link)

      @fake_file = File.join(@real_dir, 'fake_file')
      @fake_link = File.join(@link_dir, 'fake_link')

      File.symlink(@fake_file, @fake_link)
    end

    after :each do
      rm_r @file, @link, @fake_link, @real_dir, @link_dir
    end

    it "returns '/' when passed '/'" do
      File.realpath('/').should == '/'
    end

    it "returns the real (absolute) pathname not containing symlinks" do
      File.realpath(@link).should == @file
    end

    it "uses base directory for interpreting relative pathname" do
      File.realpath(File.basename(@link), @link_dir).should == @file
    end

    it "uses current directory for interpreting relative pathname" do
      Dir.chdir @link_dir do
        File.realpath(File.basename(@link)).should == @file
      end
    end

    it "raises a Errno::ELOOP if the symlink points to itself" do
      File.unlink @link
      File.symlink(@link, @link)
      lambda { File.realpath(@link) }.should raise_error(Errno::ELOOP)
    end

    it "raises Errno::ENOENT if the file is absent" do
      lambda { File.realpath(@fake_file) }.should raise_error(Errno::ENOENT)
    end

    it "raises Errno::ENOENT if the symlink points to an absent file" do
      lambda { File.realpath(@fake_link) }.should raise_error(Errno::ENOENT)
    end
  end
end
