require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.stat" do
  before :each do
    @file = '/tmp/i_exist'
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end

  it "returns a File::Stat object if the given file exists" do
    st = File.stat(@file)

    st.file?.should == true
    st.zero?.should == false
    st.size.should == 8
    st.size?.should == 8
    st.blksize.should > 0
  end
end
