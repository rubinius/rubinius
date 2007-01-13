require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# atime, basename, blockdev?, chardev?, chmod, chown, ctime, delete,
# directory?, dirname, executable?, executable_real?, exist?,
# exists?, expand_path, extname, file?, fnmatch, fnmatch?, ftype,
# grpowned?, identical?, join, lchmod, lchown, link, lstat, mtime,
# new, owned?, pipe?, readable?, readable_real?, readlink, rename,
# setgid?, setuid?, size, size?, socket?, split, stat, sticky?,
# symlink, symlink?, truncate, umask, unlink, utime, writable?,
# writable_real?, zero?

# atime, chmod, chown, ctime, flock, lstat, mtime, path, truncate

context "File class method" do
  specify "dirname should return all the components of filename except the last one" do
    example do
      [ File.dirname('/home/jason'),
        File.dirname('/home/jason/poot.txt'),
        File.dirname('poot.txt'),
        File.dirname('/holy///schnikies//w00t.bin'),
        File.dirname(''),
        File.dirname('/'),
        File.dirname('/////') ]
    end.should == ["/home", "/home/jason", ".", "/holy///schnikies", ".", "/", "/"]
  end
  
  specify "basename should return the last component of the filename" do
    example do
      [ File.basename('a'),
        File.basename('/a'),
        File.basename('/a/b'),
        File.basename('/ab/ba/bag'),
        File.basename('/ab/ba/bag.txt'),
        File.basename('/'),
        File.basename('/foo/bar/baz.rb', '.rb'),
        File.basename('baz.rb', 'z.rb') ]
    end.should == ["a", "a", "b", "bag", "bag.txt", "/", "baz", "ba"]
  end
end

context "File instance method" do
end
