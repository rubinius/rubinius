require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/fixture'

describe Object, "#fixture" do
  before :each do
    @dir = File.expand_path(Dir.pwd)
  end

  it "returns the expanded path to a fixture file" do
    name = fixture("some/path/file.rb", "dir", "file.txt")
    name.should == "#{@dir}/some/path/fixtures/dir/file.txt"
  end

  it "omits '/shared' if it is the suffix of the directory string" do
    name = fixture("some/path/shared/file.rb", "dir", "file.txt")
    name.should == "#{@dir}/some/path/fixtures/dir/file.txt"
  end
end
