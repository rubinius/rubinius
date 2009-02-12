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

  platform_is_not :windows do
    it "returns a File::Stat object with file properties for a symlink" do
      st = File.stat(@link)

      st.file?.should == true
      st.symlink?.should == false
    end
  end
end
