require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#atime" do
  before :each do
    @file = tmp('i_exist')
    touch(@file) { |f| f.write "rubinius" }
  end

  after :each do
    rm_r @file
  end
  
  it "should be able to determine the atime on a File::Stat object" do
    st = File.stat(@file)
    st.atime.class.should == Time
    st.atime.should <= Time.now
  end
end
