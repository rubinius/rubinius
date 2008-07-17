describe :file_stat, :shared => true do
  before :each do
    @file = tmp('/i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end

  it "returns a File::Stat object if the given file exists" do
    st = File.send(@method, @file)

    st.file?.should == true
    st.zero?.should == false
    st.size.should == 8
    st.size?.should == 8
    st.blksize.should > 0
    st.atime.class.should == Time
    st.ctime.class.should == Time
    st.mtime.class.should == Time
  end

  it "should be able to use the instance methods" do
    st = File.new(@file).send(@method)

    st.file?.should == true
    st.zero?.should == false
    st.size.should == 8
    st.size?.should == 8
    st.blksize.should > 0
    st.atime.class.should == Time
    st.ctime.class.should == Time
    st.mtime.class.should == Time
  end

  it "raises an Errno::ENOENT if the file does not exist" do
    lambda {
      File.send(@method, "fake_file")
    }.should raise_error(Errno::ENOENT)
  end
end
