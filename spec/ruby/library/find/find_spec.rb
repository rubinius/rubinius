require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)
require 'find'

describe "Find.find" do
  it "needs to be reviewed for spec completeness"

  before :all do
    FindDirSpecs.create_mock_dirs
  end

  after :all do
    FindDirSpecs.delete_mock_dirs
  end

  describe "when called without a block" do
    ruby_version_is ""..."1.9" do
      it "raises a LocalJumpError" do
        lambda { Find.find(FindDirSpecs.mock_dir) }.should raise_error(LocalJumpError)
      end
    end

    ruby_version_is "1.9" do
      it "returns an Enumerator" do
        Find.find(FindDirSpecs.mock_dir).should be_an_instance_of(enumerator_class)
        Find.find(FindDirSpecs.mock_dir).to_a.sort.should == FindDirSpecs.expected_paths
      end
    end
  end

  it "should recursively yield every file in the directory" do
    a = []

    Find.find(FindDirSpecs.mock_dir) do |file|
      a << file
    end

    a.sort.should == FindDirSpecs.expected_paths
  end
end
