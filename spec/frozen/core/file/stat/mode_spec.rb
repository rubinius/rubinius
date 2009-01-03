require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#mode" do
  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
    File.chmod(0755, @file)
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end
  
  it "should be able to determine the mode through a File::Stat object" do
    st = File.stat(@file)
    st.mode.is_a?(Integer).should == true
    st.mode.should == 33261
  end
end
