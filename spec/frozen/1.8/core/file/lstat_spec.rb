require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/stat'

describe "File.lstat" do
  it_behaves_like :file_stat, :lstat
end

describe "File.lstat" do
  
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
  
  it "returns a File::Stat object with symlink properties for a symlink" do
    st = File.lstat(@link)

    st.symlink?.should == true
    st.file?.should == false
  end
end
