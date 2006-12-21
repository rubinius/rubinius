require File.dirname(__FILE__) + '/helper'

class TestFile < RubiniusTestCase
  def test_file_dirname
    out = rp <<-CODE
      puts File.dirname('/home/jason')
      puts File.dirname('/home/jason/poot.txt')
      puts File.dirname('poot.txt')
      puts File.dirname('/holy///schnikies//w00t.bin')
      puts File.dirname('')
      puts File.dirname('/')
      puts File.dirname('/////')
    CODE
    assert_equal ["/home", "/home/jason", ".", "/holy///schnikies", ".", "/", "/"], out

    assert_raise TypeError do
      File.dirname(nil)
    end
  end

  def test_file_basename
    out = rp <<-CODE
      puts File.basename('a')
      puts File.basename('/a')
      puts File.basename('/a/b')
      puts File.basename('/ab/ba/bag')
      puts File.basename('/ab/ba/bag.txt')
      puts File.basename('/')
      puts File.basename('/foo/bar/baz.rb', '.rb')
      puts File.basename('baz.rb', 'z.rb')
    CODE
    assert_equal ["a", "a", "b", "bag", "bag.txt", "/", 'baz', 'ba'], out

    assert_raise TypeError do
      File.basename(nil)
    end
  end
end

