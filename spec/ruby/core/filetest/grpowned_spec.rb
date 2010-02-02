require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/grpowned'

describe "FileTest.grpowned?" do
  it_behaves_like :file_grpowned, :grpowned?, FileTest

  it "returns false if the file doesn't exist" do
    FileTest.grpowned?("xxx-tmp-doesnt_exist-blah").should be_false
  end
end
