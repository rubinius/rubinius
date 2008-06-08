require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/file'

describe "File" do
  it "includes File::Constants" do
    File.include?(File::Constants).should == true
  end
end

describe "File.file?" do
  it_behaves_like :file_file, :file?, File
end
