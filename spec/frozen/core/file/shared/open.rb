require File.dirname(__FILE__) + '/../../dir/fixtures/common'

describe :open_directory, :shared => true do
  it "opens directories" do
    lambda { File.send(@method, DirSpecs.mock_dir) }.should_not raise_error
    File.directory?(DirSpecs.mock_dir).should == true
  end
end