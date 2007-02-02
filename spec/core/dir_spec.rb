# PRIMITIVE Dir testing. Add cases as they come along.
require File.dirname(__FILE__) + '/../spec_helper'

# Class methods:
#
#   .[]             OK - NEEDS MORE
#   .chdir          OK
#   .chroot         NOT ENABLED BY DEFAULT
#   .delete         OK
#   .entries        OK
#   .foreach        OK
#   .getwd          OK
#   .glob           OK - NEEDS MORE
#   .mkdir          OK - NEEDS MORE
#   .new            OK
#   .open           OK - REVISIT
#   .pwd            OK
#   .rmdir          OK
#   .tmpdir         NOT IN MRI
#   .unlink         OK
#
# Instance methods:
#
#   #close          OK
#   #each           OK
#   #path           OK
#   #pos            OK
#   #pos=           OK
#   #read           OK
#   #rewind         OK 
#   #seek           OK
#   #tell           OK
#

require 'fileutils'

# Helpers
$mockdir = '/tmp/rubinius_mock_fs'
$nonexisting = "/rubinius_nonexist_#{$$}_00"

while File.exist? $nonexisting
  $nonexisting = $nonexisting.succ
end


def setup_mock_fs()
  FileUtils.mkdir_p "#{$mockdir}"
  FileUtils.mkdir_p "#{$mockdir}/subdir_one"
  FileUtils.mkdir_p "#{$mockdir}/subdir_two"
  FileUtils.mkdir_p "#{$mockdir}/.dotsubdir"
  FileUtils.mkdir_p "#{$mockdir}/deeply/nested/directory/structure"

  FileUtils.touch "#{$mockdir}/.dotfile"
  FileUtils.touch "#{$mockdir}/nondotfile"
  FileUtils.touch "#{$mockdir}/file_one.ext"
  FileUtils.touch "#{$mockdir}/file_two.ext"
  FileUtils.touch "#{$mockdir}/subdir_one/.dotfile"
  FileUtils.touch "#{$mockdir}/subdir_one/nondotfile"
  FileUtils.touch "#{$mockdir}/subdir_two/nondotfile"
  FileUtils.touch "#{$mockdir}/subdir_two/nondotfile.ext"
  FileUtils.touch "#{$mockdir}/deeply/.dotfile"
  FileUtils.touch "#{$mockdir}/deeply/nondotfile"
  FileUtils.touch "#{$mockdir}/deeply/nested/.dotfile.ext"
  FileUtils.touch "#{$mockdir}/deeply/nested/directory/structure/file_one.ext"
  FileUtils.touch "#{$mockdir}/deeply/nested/directory/structure/file_one"
  FileUtils.touch "#{$mockdir}/deeply/nested/directory/structure/foo"
  FileUtils.touch "#{$mockdir}/deeply/nested/directory/structure/bar"
  FileUtils.touch "#{$mockdir}/deeply/nested/directory/structure/baz"
  FileUtils.touch "#{$mockdir}/deeply/nested/directory/structure/.ext"
end

#def teardown_mock_fs()
#end

warn "Running Dir specs will leave you with a #{$mockdir}, feel free to delete it."
setup_mock_fs

warn 'Dir specs are incomplete. Please add corner cases.'


context 'Using Dir to move around the filesystem' do
  specify 'Dir.pwd and Dir.getwd return the current working directory' do
    example do
      [Dir.pwd, Dir.getwd]
    end.should == ([`pwd`.chomp] * 2)
  end

  specify 'Dir.chdir can be used to change the working directory--temporary if a block is provided. Defaults to $HOME' do
    example do
      orig = Dir.pwd

      Dir.chdir; a = Dir.pwd; Dir.chdir orig
      Dir.chdir $mockdir; b = Dir.pwd; Dir.chdir orig 
      Dir.chdir($mockdir) {|dir| @c = [dir, Dir.pwd]}; d = Dir.pwd

      # Return values
      e = Dir.chdir orig
      f = Dir.chdir(orig) {:returns_block_value}

      g = begin
            Dir.chdir $nonexisting
          rescue Exception => x
            x.class < SystemCallError
          end

      [a, b, @c, d, e, f, g]
    end.should == [ENV['HOME'], $mockdir, [$mockdir, $mockdir], Dir.pwd, 0, :returns_block_value, true]
  end

  # Need special perms to run chroot
#  specify 'Dir.chroot can be used to change the process\' root directory, see chroot(2)' do
#    example do
#      Kernel.fork {
#        begin
#          ret = Dir.chroot $mockdir
#          File.open('/root_contents.txt', 'wb') {|f| f.puts ret; f.puts Dir.entries('/').sort}
#          FileUtils.chmod 0777, '/root_contents.txt'
#        rescue SystemCallError
#          warn '**WARN: Insufficient permissions to test Dir.chroot! (Not a huge problem.)'
#        end
#      }
#
#      Process.waitall
#  
#      contents = File.read "#{$mockdir}/root_contents.txt"
#      FileUtils.rm "#{$mockdir}/root_contents.txt"
#      
#      # Should have the return value + the filenames
#      contents.split("\n").sort
#    end.should == %w|0 . .. .dotfile .dotsubdir subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext root_contents.txt|.sort
#  end

  # Not enough info to ensure this is correct
#  specify 'Dir.tempdir points to the system\'s temporary directory' do
#    example do
#      [ENV['TMPDIR'], ENV['TEMPDIR'], '/tmp'].select {|dir| 
#        dir and File.directory?(dir) and File.writable?(dir)
#      }.include? Dir.tempdir 
#    end.should == true
#  end
end

context 'Using Dir to modify the filesystem' do
  setup do 
    @orig = Dir.pwd
    Dir.chdir $mockdir
  end

  teardown do 
    Dir.chdir @orig
  end

  specify 'Dir.mkdir creates the named directory with the given permissions' do
    example do
      a = File.exist? 'nonexisting'
      Dir.mkdir 'nonexisting'
      b = File.exist? 'nonexisting'
      
      Dir.mkdir 'default_perms'
      c = File.stat('default_perms').mode
      Dir.mkdir 'reduced', (c - 1)
      d = File.stat('reduced').mode

      e = Dir.mkdir 'always_returns_0'

      FileUtils.chmod 0777, %w|nonexisting default_perms reduced always_returns_0|
      Dir.rmdir 'nonexisting'
      Dir.rmdir 'default_perms'
      Dir.rmdir 'reduced'
      Dir.rmdir 'always_returns_0'

      [a, b, (c != d), e]
    end.should == [false, true, true, 0]
  end

  specify 'Dir.mkdir raises without adequate permissions in the parent dir' do
    example do
      Dir.mkdir 'noperms', 0000

      a = begin
            Dir.mkdir "noperms/subdir"
          rescue Exception => x

            x.class < SystemCallError
          end

      FileUtils.chmod 0777, 'noperms'
      Dir.rmdir 'noperms'

      a
    end.should == true
  end

  specify 'Dir.mkdir cannot create directory hierarchies' do
    example do
      begin
        Dir.mkdir "#{$nonexisting}/subdir"
      rescue Exception => x
        x.class < SystemCallError
      end
    end.should == true
  end

  specify 'Dir.rmdir, .delete and .unlink remove non-empty directories' do
    %w|rmdir delete unlink|.each {|cmd|
      example do
        Dir.mkdir 'empty_subdir'
        Dir.send(cmd, 'empty_subdir') 
      end.should == 0
    }
  end

  specify 'Dir.rmdir, .delete and .unlink will raise an exception trying to remove a nonempty directory' do
    %w|rmdir delete unlink|.each {|cmd|
      example do
        begin
          Dir.send(cmd, 'subdir_one') 
        rescue Exception => x
          x.class < SystemCallError
        end
      end.should == true
    }
  end

  specify 'Dir.rmdir, .delete and .unlink need adequate permissions to remove a directory or will raise' do
    %w|rmdir delete unlink|.each {|cmd|
      example do
        FileUtils.mkdir_p     "noperm_#{cmd}/child"
        FileUtils.chmod 0000, "noperm_#{cmd}"

        a = begin
              Dir.send(cmd, "noperm_#{cmd}/child") 
            rescue Exception => x
              x.class < SystemCallError
            end

        FileUtils.chmod 0777, "noperm_#{cmd}"
        Dir.rmdir "noperm_#{cmd}/child"
        Dir.rmdir "noperm_#{cmd}"

        a
      end.should == true
    }
  end
end

context 'Examining directory contents with Dir' do
  specify 'Dir.entries gives an Array of filenames in an existing directory including dotfiles' do
    example do
      a = Dir.entries $mockdir
      b = Dir.entries "#{$mockdir}/deeply/nested"

      c = begin 
            Dir.entries $nonexisting
          rescue Exception => x
            x.class < SystemCallError
          end

      [a.sort, b.sort, c]
    end.should == [%w|. .. subdir_one subdir_two .dotsubdir deeply .dotfile nondotfile file_one.ext file_two.ext|.sort,
                   %w|. .. .dotfile.ext directory|.sort,
                   true]
  end

  specify 'Dir.foreach yields all filenames (including dotfiles) in an existing directory to block provided, returns nil' do
    example do
      a, b = [], []
     
      Dir.foreach($mockdir) {|f| a << f}
      Dir.foreach("#{$mockdir}/deeply/nested") {|f| b << f}

      c = begin 
            Dir.foreach $nonexisting
          rescue Exception => x
            x.class < SystemCallError
          end

      d = Dir.foreach($mockdir) {|f| f}

      [a.sort, b.sort, c, d]
    end.should == [%w|. .. subdir_one subdir_two .dotsubdir deeply .dotfile nondotfile file_one.ext file_two.ext|.sort,
                   %w|. .. .dotfile.ext directory|.sort,
                   true, nil]
  end
end

context 'Wildcard-matching directory contents with Dir.glob (Dir[PATTERN] is equivalent to Dir.glob(PATTERN, 0)' do
  setup do 
    @orig = Dir.pwd
    Dir.chdir $mockdir
  end

  teardown do 
    Dir.chdir @orig
  end

  specify "* by itself matches any non-dotfile" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg,'*').sort
    end.should == %w|subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
  }
  end

  specify ".* by itself matches any dotfile" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '.*').sort
    end.should == %w|. .. .dotfile .dotsubdir|.sort
  }
  end

  specify "* with option File::FNM_DOTMATCH matches both dot- and nondotfiles" do
    example do
      Dir.glob('*', File::FNM_DOTMATCH).sort
    end.should == %w|. .. .dotfile .dotsubdir subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
  end

  specify "* followed by literals matches any (or no) beginning for nondot filenames" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '*file').sort
    end.should == %w|nondotfile|.sort
  }
  end

  specify ".* followed by a string matches any (or no) beginning for dotfile names" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '.*file').sort
    end.should == %w|.dotfile|.sort
  }
  end

  specify "* with File::FNM_DOTMATCH followed by literals matches any (or no) beginning for any filenames" do
    example do
      Dir.glob('*file', File::FNM_DOTMATCH).sort
    end.should == %w|.dotfile nondotfile|.sort
  end

  specify "* in the end of a string matches any (or no) ending" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, 'file*').sort
    end.should == %w|file_one.ext file_two.ext|.sort
  }
  end

  specify "* in the middle matches any (or no) characters" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, 'sub*_one').sort
    end.should == %w|subdir_one|.sort
  }
  end

  specify "multiple * may appear in a glob to use all above capabilities" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '*fi*e*').sort
    end.should == %w|nondotfile file_one.ext file_two.ext|.sort
  }
  end

  specify "** by itself matches any nondot files in the current directory" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '**').sort
    end.should == %w|subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
  }
  end

  specify ".** by itself matches any dotfiles in the current directory" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '.**').sort
    end.should == %w|. .. .dotsubdir .dotfile|.sort
  }
  end

  specify ".** with File::FNM_DOTMATCH matches any files in the current directory" do
    example do
      Dir.glob('**', File::FNM_DOTMATCH).sort
    end.should == %w|. .. .dotsubdir .dotfile subdir_one subdir_two deeply nondotfile file_one.ext file_two.ext|.sort
  end

  specify "**/ recursively matches any nondot subdirectories" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '**/').sort
    end.should == %w|subdir_one/ subdir_two/ deeply/ deeply/nested/ 
                     deeply/nested/directory/ deeply/nested/directory/structure/|.sort
  }
  end

  specify ".**/ recursively matches any subdirectories including ./ and ../" do
  %w|glob []|.each {|msg|
    example do
      Dir.chdir "#{$mockdir}/subdir_one"
      Dir.send(msg, '.**/').sort
    end.should == %w|./ ../|.sort
  }
  end

  specify "**/ with File::FNM_DOTMATCH recursively matches any subdirectories (not ./ or ../)" do
    example do
      Dir.glob('**/', File::FNM_DOTMATCH).sort
    end.should == %w|.dotsubdir/ subdir_one/ subdir_two/ deeply/ deeply/nested/ 
                     deeply/nested/directory/ deeply/nested/directory/structure/|.sort
  end

  specify "? can be used anywhere in a file name to match any one character except leading ." do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '?ubdir_one').sort
    end.should == %w|subdir_one|.sort
  }
  end

  specify "multiple ? can appear to match any one character each" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, 'subdir_???').sort
    end.should == %w|subdir_one subdir_two|.sort
  }
  end

  specify "[CHARACTERS] can be used to match any one character of the ones in the brackets" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '[stfu]ubdir_one').sort
    end.should == %w|subdir_one|.sort
  }
  end

  specify "[CHAR-OTHER] can contain ranges of characters such as a-z" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '[a-zA-Z]ubdir_one').sort
    end.should == %w|subdir_one|.sort
  }
  end

  specify "[^CHARACTERS] matches anything BUT those characters or range" do
  %w|glob []|.each {|msg|
    example do
      [Dir.send(msg, '[^wtf]ubdir_one').sort,
       Dir.send(msg, '[^0-9]ubdir_one').sort]
    end.should == [%w|subdir_one|.sort, %w|subdir_one|.sort]
  }
  end

  specify "[^CHAR-OTHER] matches anything BUT those characters or range" do
  %w|glob []|.each {|msg|
    example do
      [Dir.send(msg, '[^wtf]ubdir_one').sort,
       Dir.send(msg, '[^0-9]ubdir_one').sort]
    end.should == [%w|subdir_one|.sort, %w|subdir_one|.sort]
  }
  end

  specify "{string,other,...} matches one of the provided strings exactly in any part of a filename" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, 'subdir_{one,two,three}').sort
    end.should == %w|subdir_one subdir_two|.sort
  }
  end

  specify "{string,,other} can have an empty string" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, 'deeply/nested/directory/structure/file_one{.ext,}').sort
    end.should == %w|deeply/nested/directory/structure/file_one.ext
                     deeply/nested/directory/structure/file_one|.sort
  }
  end

  specify "{,.}* etc. can be used to match any dot- or nondot file" do
  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '{,.}*').sort
    end.should == %w|. .. .dotsubdir subdir_one subdir_two deeply .dotfile nondotfile file_one.ext file_two.ext|.sort
  }
  end

  specify 'In a single-quoted pattern, \ escapes the following character of any special meaning' do
  %w|glob []|.each {|msg|
    example do
      Dir.mkdir 'foo*bar'
      
      a = Dir.glob 'foo?bar'
      b = Dir.glob 'foo\?bar'
      c = Dir.glob 'nond\otfile'

      Dir.rmdir 'foo*bar'

      [a, b, c]
    end.should == [%w|foo*bar|, [], %w|nondotfile|]
  }
  end

  specify 'In a single-quoted pattern, File::FNM_NOESCAPE treats \ as the literal backslash' do
    example do
      Dir.mkdir 'foo?bar'
      
      a = Dir.glob 'foo?bar', File::FNM_NOESCAPE
      b = Dir.glob 'foo\?bar', File::FNM_NOESCAPE

      Dir.mkdir 'foo\?bar'

      c = Dir.glob 'foo\?bar', File::FNM_NOESCAPE

      Dir.rmdir 'foo?bar'
      Dir.rmdir 'foo\?bar'

      [a, b, c]
    end.should == [%w|foo?bar|, [], %w|foo\\?bar|]
  end

  specify 'Normally, / is a special character. File::FNM_PATHNAME treats it like any regular character' do
    example do
      # There is no meaningful use of File::FNM_PATHNAME in glob. See File.fnmatch for proper use
      a = Dir.glob 'subdir_one/nondotfile'
      b = Dir.glob 'subdir_one/nondotfile', File::FNM_PATHNAME
      
      a == b
    end.should == true
  end

  specify "**/PATTERN recursively matches the pattern (as above) in itself and all subdirectories" do
    warn 'MRI (glob(3)) and shell glob return deeply/nested/directory/structure/file_one.ext twice!' 

  %w|glob []|.each {|msg|
    example do
      Dir.send(msg, '**/*fil?{,.}*').sort
    end.should == %w|nondotfile file_one.ext file_two.ext subdir_one/nondotfile subdir_two/nondotfile
                     subdir_two/nondotfile.ext deeply/nondotfile deeply/nested/directory/structure/file_one.ext
                     deeply/nested/directory/structure/file_one|.sort
  }
  end
end


context 'Creating Dir objects' do
  specify 'Both Dir.new and Dir.open return a new Dir instance' do
    example do
      a = Dir.new($mockdir)
      b = Dir.open($mockdir)

      res = [a.class, b.class]

      a.close
      b.close

      res
    end.should == [Dir, Dir]
  end

  specify 'Dir.new and Dir.open will raise if the directory does not exist' do
  %w|new open|.each {|msg|
    example do
      begin
        Dir.send msg, $nonexisting
      rescue SystemCallError
        :failure
      end
    end.should == :failure
  }
  end

  specify 'Dir.open may also take a block which yields the Dir instance and closes it after. Returns block value' do
    # This is a bit convoluted but we are trying to ensure the file gets closed.
    # To do that, we peek to see what the next FD number is and then probe that
    # to see whether it has been closed.
    example do
      peek = IO.sysopen $mockdir
      File.for_fd(peek).close

      res = Dir.open($mockdir) {|dir| File.for_fd peek; dir.class}  # Should be open here
      test = File.for_fd(peek) rescue :success                      # And closed here
  
      [res, test]      
    end.should == [Dir, :success]
  end
end

dir_spec_object_text = <<END 
Using Dir objects

#  The pointer associated with an open directory is not like an Array
#  index, more like an instruction sequence pointer. Pretty much any
#  operation below increments the pointer. It is, for example, possible
#  that both positions 1 and 27 refer to the point where the next read
#  will be the first entry in the directory. So the position number is
#  not as important as the /time at which it was obtained/.

END

context dir_spec_object_text do
  setup do
    @dir = Dir.open $mockdir
  end

  teardown do
    @dir.close rescue nil
  end

  specify 'Dir#path gives the path that was supplied to .new or .open' do
    example do
      @dir.path
    end.should == $mockdir
  end

  specify 'Dir#read gives the file name in the current seek position' do
    example do
      @dir.read
    end.should == '.'
  end

  specify 'Both Dir#pos and Dir#tell give the current dir position' do
    example do
      a = @dir.pos
      b = @dir.tell
      c = @dir.pos
      d = @dir.tell

      [a, b, c, d]
    end.should == [1, 2, 3, 4]
  end

  specify 'Dir#seek can be used to return to a certain position (obtained from #pos or #tell), returns the Dir object' do
    example do
      pos = @dir.pos
      a   = @dir.read
      b   = @dir.read
      ret = @dir.seek pos
      c   = @dir.read
      
      [a != b, b != c, c == a, ret.class]
    end.should == [true, true, true, Dir]
  end

  specify 'Dir#pos= also seeks to a certain position but returns the position number instead' do
    example do
      pos = @dir.pos
      a   = @dir.read
      b   = @dir.read
      ret = @dir.pos = pos
      c   = @dir.read
      
      [a != b, b != c, c == a, ret == pos]
    end.should == [true, true, true, true]
  end

  specify 'Dir#rewind will reset the next read to start from the first entry but *does not reset the pointer to 1*' do
    example do
      first   = @dir.pos
      a       = @dir.read
      b       = @dir.read
      prejmp  = @dir.pos
      ret     = @dir.rewind
      second  = @dir.pos
      c       = @dir.read
      
      [a != b, b != c, c == a, 
       ret.class, 
       second != first, second != prejmp]
    end.should == [true, true, true, Dir, true, true]
  end

  specify 'Dir#each will yield each directory entry in succession' do
    example do
      a = []
      @dir.each {|dir| a << dir}
      a.sort
    end.should == %w|. .. .dotfile .dotsubdir nondotfile subdir_one subdir_two deeply file_one.ext file_two.ext|.sort
  end

  specify 'Dir#each returns the directory which remains open' do
    example do
      dir = @dir.each {}
      cur = @dir.read
      @dir.rewind
      yay = @dir.read

      [dir == @dir, cur == nil, yay == '.']
    end.should == [true, true, true]
  end

  specify 'Dir#close will close the stream and fd and returns nil' do
    # This is a bit convoluted but we are trying to ensure the file gets closed.
    # To do that, we peek to see what the next FD number is and then probe that
    # to see whether it has been closed.
    example do
      peek = IO.sysopen $mockdir
      File.for_fd(peek).close

      dir = Dir.open $mockdir
      File.for_fd peek                          # Should be open here
      ret = dir.close
      res = File.for_fd(peek) rescue :success   # And closed here

      [ret, res]
    end.should == [nil, :success]
  end

  specify 'Further attempts to use a dir that has been #closed will result in an error' do
  %w|close each path pos read rewind tell|.each {|msg|
    example do
      begin
        dir = Dir.open $mockdir
        dir.close
        dir.send msg
      rescue Exception
        :success
      end
    end.should == :success
  }
  end
end
