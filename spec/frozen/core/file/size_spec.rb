require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/size'

describe "File.size" do
  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end

  it "returns the size of the file" do
    File.size?(@file).should == 8
  end

  it "accepts a String-like (to_str) parameter" do
    o = [@file]
    def o.to_str; self[0]; end

    File.size?(o).should == 8
  end

  it "accepts a File argument" do
    File.size?(File.open(@file)).should == 8
  end
end

describe "File.size?" do
  it_behaves_like :file_size, :size?, File
  it_behaves_like :file_size_missing, :size?, File
end
