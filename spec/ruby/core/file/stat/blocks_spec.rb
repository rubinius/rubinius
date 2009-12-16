require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#blocks" do
  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end
  
  it "should be able to determine the blocks on a File::Stat object" do
    st = File.stat(@file)
    st.blocks.is_a?(Integer).should == true
    st.blocks.should > 0
  end
end
