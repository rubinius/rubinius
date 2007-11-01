require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.stat" do
  before :each do
    @file = '/tmp/i_exist'
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end

  it "should return a File::Stat object if the given file exists" do
    File.stat(@file).file?.should == true
  end
end
