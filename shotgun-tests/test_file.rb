require File.dirname(__FILE__) + '/helper'

class TestFile < RubiniusTestCase
  #TypeError: can't convert nil into String
  def test_file_dirname
    out = rp <<-CODE
      puts File.dirname('/home/jason')
      puts File.dirname('/home/jason/poot.txt')
      puts File.dirname('poot.txt')
      puts File.dirname('/holy///schnikies//w00t.bin');
      puts File.dirname('')
      puts File.dirname('/')
      puts File.dirname('/////')
    CODE
    assert_equal ["/home", "/home/jason", ".", "/holy///schnikies", ".", "/", "/"], out

    assert_raise TypeError do
      File.dirname(nil)
    end
  end
end

