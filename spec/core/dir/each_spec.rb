require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/closed'

describe "Dir#each" do
  before(:each) do
    @dir = Dir.open mock_dir
  end
  
  after(:each) do
    @dir.close
  end

  it "yields each directory entry in succession" do
    a = []
    @dir.each {|dir| a << dir}
    a.sort.should == %w|. .. .dotfile .dotsubdir nondotfile subdir_one subdir_two deeply file_one.ext file_two.ext|.sort
  end

  it "returns the directory which remains open" do
    @dir.each {}.should == @dir
    @dir.read.should == nil
    @dir.rewind
    @dir.read.should == '.'
  end
end

describe "Dir#each" do
  it_behaves_like @dir_closed, :each
end
