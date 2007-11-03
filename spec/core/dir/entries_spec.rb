require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Dir.entries" do
  it "returns an Array of filenames in an existing directory including dotfiles" do

    Dir.entries(mock_dir).sort.should == %w|. .. subdir_one subdir_two .dotsubdir deeply 
                                            .dotfile nondotfile file_one.ext file_two.ext foo*bar foo?bar|.sort

    Dir.entries("#{mock_dir}/deeply/nested").sort.should == %w|. .. .dotfile.ext directory|.sort
  end
  
  it "raises a SystemCallError if called with a nonexistent diretory" do
    should_raise(SystemCallError) { Dir.entries nonexistent }
  end
end
