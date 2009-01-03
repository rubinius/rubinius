require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/closed'

describe "Dir#path" do
  it "returns the path that was supplied to .new or .open" do
    dir = Dir.open DirSpecs.mock_dir
    dir.path.should == DirSpecs.mock_dir
    dir.close rescue nil
  end
end

describe "Dir#path" do
  ruby_version_is ""..."1.8.7" do
    it "raises an IOError when called on a closed Dir instance" do
      lambda {
        dir = Dir.open DirSpecs.mock_dir
        dir.close
        dir.path
      }.should raise_error(IOError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns the path even when called on a closed Dir instance" do
      dir = Dir.open DirSpecs.mock_dir
      dir.close
      dir.path.should == DirSpecs.mock_dir
    end
  end
end
