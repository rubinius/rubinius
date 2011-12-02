require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "File.realpath" do
    before do
      @real_dir = tmp('dir_realpath_real')
      @link_dir = tmp('dir_realpath_link')

      mkdir_p @real_dir
      File.symlink(@real_dir, @link_dir)

      @file = File.join(@real_dir, 'file')
      @link = File.join(@link_dir, 'link')

      touch @file
      File.symlink(@file, @link)
    end

    after do
      File.unlink @link
      File.unlink @link_dir
      rm_r @file, @real_dir
    end

    it "returns root for root" do
      File.realpath('/').should == '/'
    end

    it "returns the real (absolute) pathname not containing symlinks" do
      File.realpath(@link).should == @file
    end

    it "uses base directory for interpreting relative pathname" do
      File.realpath(File.basename(@link), @link_dir).should == @file
    end

    it "uses current directory for interpreting relative pathname" do
      old_dir = Dir.pwd
      begin
        Dir.chdir @link_dir
        File.realpath(File.basename(@link)).should == @file
      ensure
        Dir.chdir old_dir
      end
    end
  end
end
