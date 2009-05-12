describe :file_grpowned, :shared => true do
  before :each do
    @file = tmp('i_exist')
    @fh = File.open(@file,'w') { |f| f.write 'rubinius' }
    File.chown(nil, Process.gid, @file) rescue nil
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end

  platform_is_not :windows do
    it "return true if the file exist" do
      @object.send(@method, @file).should == true
    end
  end

  platform_is :windows do
    it "return false if the file exist" do
      @object.send(@method, @file).should == false
    end
  end
end
