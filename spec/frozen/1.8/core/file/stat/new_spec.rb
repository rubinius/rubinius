require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#initialize" do

  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
    File.chmod(0755, @file)
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end

  it "raises an exception if the file doesn't exist" do
    lambda { File::Stat.new(tmp("i_am_a_dummy_file_that_doesnt_exist")) }.should raise_error
  end

  it "creates a File::Stat object for the given file" do
    st = File::Stat.new(@file)
    st.class.should == File::Stat
    st.ftype.should == 'file'
  end

end
