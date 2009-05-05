describe :file_symlink, :shared => true do
  before :each do
    @file = "test.txt"
    @link = "test.lnk"
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
    File.open(@file,"w+")
  end

  after :each do
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
  end

  platform_is_not :windows do
    it "returns true if the file is a link" do
      File.symlink(@file, @link)
      @object.send(@method, @link).should == true
    end
  end
end

describe :file_symlink_nonexistent, :shared => true do
  before :each do
    @file = "test.txt"
    @link = "test.lnk"
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
    File.open(@file,"w+")
  end

  after :each do
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
  end

  platform_is_not :windows do
    it "returns false if the file does not exist" do
      @object.send(@method, "non_existent_link").should == false
    end
  end
end
