
shared :dir_glob do |cmd|
  describe "Dir.#{cmd}" do
    before(:all) do
      @cwd = Dir.pwd
      Dir.chdir DirSpecs.mock_dir
    end
    
    it "matches non-dotfiles with '*'" do
      Dir.send(cmd,'*').sort.should == 
        %w|subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
    end

    it "matches dotfiles with '.*'" do
      Dir.send(cmd, '.*').sort.should == %w|. .. .dotfile .dotsubdir|.sort
    end

    it "matches non-dotfiles with '*<non-special characters>'" do
      Dir.send(cmd, '*file').sort.should == %w|nondotfile|.sort
    end

    it "matches dotfiles with '.*<non-special characters>'" do
      Dir.send(cmd, '.*file').sort.should == %w|.dotfile|.sort
    end

    it "matches files with any ending with '<non-special characters>*'" do
      Dir.send(cmd, 'file*').sort.should == %w|file_one.ext file_two.ext|.sort
    end

    it "matches files with any middle with '<non-special characters>*<non-special characters>'" do
      Dir.send(cmd, 'sub*_one').sort.should == %w|subdir_one|.sort
    end

    it "matches files with multiple '*' special characters" do
      Dir.send(cmd, '*fi*e*').sort.should == %w|nondotfile file_one.ext file_two.ext|.sort
    end

    it "matches non-dotfiles in the current directory with '**'" do
      Dir.send(cmd, '**').sort.should == %w|subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
    end

    it "matches dotfiles in the current directory with '.**'" do
      Dir.send(cmd, '.**').sort.should == %w|. .. .dotsubdir .dotfile|.sort
    end

    it "recursively matches any nondot subdirectories with '**/'" do
      Dir.send(cmd, '**/').sort.should == %w|subdir_one/ subdir_two/ deeply/ deeply/nested/ 
                                             deeply/nested/directory/ deeply/nested/directory/structure/|.sort
    end

    it "recursively matches any subdirectories including ./ and ../ with '.**/'" do
      Dir.chdir("#{DirSpecs.mock_dir}/subdir_one") do
        Dir.send(cmd, '.**/').sort.should == %w|./ ../|.sort
      end
    end

    it "matches a single character except leading '.' with '?'" do
      Dir.send(cmd, '?ubdir_one').sort.should == %w|subdir_one|.sort
    end

    it "accepts multiple '?' characters in a pattern" do
      Dir.send(cmd, 'subdir_???').sort.should == %w|subdir_one subdir_two|.sort
    end

    it "matches any characters in a set with '[<characters>]'" do
      Dir.send(cmd, '[stfu]ubdir_one').sort.should == %w|subdir_one|.sort
    end

    it "matches any characters in a range with '[<character>-<character>]'" do
      Dir.send(cmd, '[a-zA-Z]ubdir_one').sort.should == %w|subdir_one|.sort
    end

    it "matches any characters except those in a set with '[^<characters>]'" do
      Dir.send(cmd, '[^wtf]ubdir_one').sort.should == %w|subdir_one|.sort
    end

    it "matches any characters except those in a range with '[^<character>-<character]'" do
      Dir.send(cmd, '[^0-9]ubdir_one').sort.should == %w|subdir_one|.sort
    end

    it "matches any one of the strings in a set with '{<string>,<other>,...}'" do
      Dir.send(cmd, 'subdir_{one,two,three}').sort.should == %w|subdir_one subdir_two|.sort
    end

    it "accepts string sets with empty strings with {<string>,,<other>}" do
      a = Dir.send(cmd, 'deeply/nested/directory/structure/file_one{.ext,}').sort
      a.should == %w|deeply/nested/directory/structure/file_one.ext
                     deeply/nested/directory/structure/file_one|.sort
    end

    it "matches dot or non-dotfiles with '{,.}*'" do
      Dir.send(cmd, '{,.}*').sort.should == %w|. .. .dotsubdir subdir_one subdir_two deeply 
                                               .dotfile nondotfile file_one.ext file_two.ext|.sort
    end

    it "matches special characters by escaping with a backslash with '\\<character>'" do
      Dir.mkdir 'foo*bar'

      Dir.glob('foo?bar').should == %w|foo*bar|
      Dir.glob('foo\?bar').should == []
      Dir.glob('nond\otfile').should == %w|nondotfile| 

      Dir.rmdir 'foo*bar'
    end

    it "recursively matches directories with '**/<characters>'" do
      %w|glob []|.each {|cmd|
        Dir.send(cmd, '**/*fil?{,.}*').sort.should == %w|deeply/nested/directory/structure/file_one 
                                                         deeply/nested/directory/structure/file_one.ext 
                                                         deeply/nondotfile file_one.ext file_two.ext 
                                                         nondotfile subdir_one/nondotfile 
                                                         subdir_two/nondotfile subdir_two/nondotfile.ext 
                                                         subdir_two/nondotfile.ext|
      }
    end
    after(:all) do
      Dir.chdir @cwd
    end
  end
end
