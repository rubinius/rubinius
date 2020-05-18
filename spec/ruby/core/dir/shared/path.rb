require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/common', __FILE__)
require File.expand_path('../closed', __FILE__)

describe :dir_path, :shared => true do
  it "returns the path that was supplied to .new or .open" do
    dir = Dir.open DirSpecs.mock_dir
    dir.send(@method).should == DirSpecs.mock_dir
    dir.close rescue nil
  end

  it "returns the path even when called on a closed Dir instance" do
    dir = Dir.open DirSpecs.mock_dir
    dir.close
    dir.send(@method).should == DirSpecs.mock_dir
  end
end
