require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir.foreach" do
  it "yields all names in an existing directory to the provided block" do
    a, b = [], []

    Dir.foreach(DirSpecs.mock_dir) {|f| a << f}
    Dir.foreach("#{DirSpecs.mock_dir}/deeply/nested") {|f| b << f}

    a.sort.should == %w|. .. subdir_one subdir_two .dotsubdir deeply .dotfile nondotfile file_one.ext file_two.ext|.sort
    b.sort.should == %w|. .. .dotfile.ext directory|.sort
  end
  
  it "returns nil when successful" do
    Dir.foreach(DirSpecs.mock_dir) {|f| f}.should == nil
  end
  
  it "raises a SystemCallError if passed a nonexistent directory" do
    should_raise(SystemCallError) do
      Dir.foreach DirSpecs.nonexistent
    end
  end
end
