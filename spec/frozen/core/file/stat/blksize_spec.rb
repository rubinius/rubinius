require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#blksize" do
  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end
  
  it "should be able to determine the blksize on a File::Stat object" do
    st = File.stat(@file)
    st.blksize.is_a?(Integer).should == true
    st.blksize.should > 0
  end
end
