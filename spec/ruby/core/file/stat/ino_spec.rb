require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#ino" do
  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end
  
  it "should be able to determine the ino on a File::Stat object" do
    st = File.stat(@file)
    st.ino.is_a?(Integer).should == true
    st.ino.should > 0
  end
end
