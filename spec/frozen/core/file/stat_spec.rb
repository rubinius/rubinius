require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/stat'

describe "File.stat" do
  it_behaves_like :file_stat, :stat
end

describe "File.stat" do

  before :each do
    @file = tmp('i_exist')
    @link = tmp('i_am_a_symlink')
    File.open(@file,'w'){|f| f.write 'rubinius'}
    File.symlink(@file, @link)
  end

  after :each do
    File.delete(@link) if File.exist?(@link)
    File.delete(@file) if File.exist?(@file)
  end

  it "should be able to call stat on an unlinked file" do
    File.open(@file) do |f|
      File.unlink(@link)
      File.unlink(@file)
      st = f.stat

      st.file?.should == true
      st.zero?.should == false
      st.size.should == 8
      st.size?.should == 8
      st.blksize.should > 0
      st.atime.class.should == Time
      st.ctime.class.should == Time
      st.mtime.class.should == Time
    end
  end

  platform_is_not :windows do
    it "returns a File::Stat object with file properties for a symlink" do
      st = File.stat(@link)

      st.file?.should == true
      st.symlink?.should == false
    end
  end
end
