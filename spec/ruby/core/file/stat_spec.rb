require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/stat'

describe "File.stat" do
  it_behaves_like :file_stat, :stat
end

describe "File.stat" do

  before :each do
    @file = tmp('i_exist')
    @link = tmp('i_am_a_symlink')
    touch(@file) { |f| f.write "rubinius" }
    File.symlink(@file, @link)
  end

  after :each do
    rm_r @link, @file
  end

  it "returns information for a file that has been deleted but is still open" do
    File.open(@file) do |f|
      rm_r @link, @file

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
