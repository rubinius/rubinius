require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#gid" do
  before :each do
    @file = tmp('i_exist')
    touch(@file) { |f| f.write "rubinius" }
    File.chown(nil, Process.gid, @file)
  end

  after :each do
    rm_r @file
  end
  
  it "should be able to determine the group owner through a File::Stat object" do
    st = File.stat(@file)
    st.gid.is_a?(Integer).should == true
    st.gid.should == Process.gid
  end
end
