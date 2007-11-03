require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/glob'

describe "Dir.glob" do
  it_behaves_like(:dir_glob, :glob)
  
  before(:all) do
    @cwd = Dir.pwd
    Dir.chdir mock_dir
  end
  
  it "matches both dot and non-dotfiles with '*' and option File::FNM_DOTMATCH" do
    Dir.glob('*', File::FNM_DOTMATCH).sort.should == 
      %w|. .. .dotfile .dotsubdir subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
  end

  it "matches files with any beginning with '*<non-special characters>' and option File::FNM_DOTMATCH" do
    Dir.glob('*file', File::FNM_DOTMATCH).sort.should == %w|.dotfile nondotfile|.sort
  end

  it "matches any files in the current directory with '.**' and option File::FNM_DOTMATCH" do
    Dir.glob('**', File::FNM_DOTMATCH).sort.should == %w|. .. .dotsubdir .dotfile subdir_one subdir_two 
                                                         deeply nondotfile file_one.ext file_two.ext|.sort
  end

  it "recursively matches any subdirectories except './' or '../' with '**/' and option File::FNM_DOTMATCH" do
    Dir.glob('**/', File::FNM_DOTMATCH).sort.should == %w|.dotsubdir/ subdir_one/ subdir_two/ deeply/ deeply/nested/ 
                                                          deeply/nested/directory/ deeply/nested/directory/structure/|.sort
  end

  it "matches the literal character '\\' with option File::FNM_NOESCAPE" do
    Dir.mkdir 'foo?bar'
    
    Dir.glob('foo?bar', File::FNM_NOESCAPE).should == %w|foo?bar|
    Dir.glob('foo\?bar', File::FNM_NOESCAPE).should == []

    Dir.mkdir 'foo\?bar'

    Dir.glob('foo\?bar', File::FNM_NOESCAPE).should == %w|foo\\?bar|

    Dir.rmdir 'foo?bar'
    Dir.rmdir 'foo\?bar'
  end

  after(:all) do
    Dir.chdir @cwd
  end
end
