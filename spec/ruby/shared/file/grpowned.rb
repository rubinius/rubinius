describe :file_grpowned, :shared => true do
  before :each do
    @file = tmp('i_exist')
    @fh = File.open(@file,'w') { |f| f.write 'rubinius' }
    File.chown(nil, Process.gid, @file) rescue nil
  end

  after :each do
    rm_r @file
  end

  platform_is_not :windows do
    it "return true if the file exist" do
      @object.send(@method, @file).should == true
    end

    ruby_version_is "1.9" do
      it "accepts an object that has a #to_path method" do
        @object.send(@method, mock_to_path(@file)).should == true
      end
    end
  end

  platform_is :windows do
    it "return false if the file exist" do
      @object.send(@method, @file).should == false
    end
  end
end
