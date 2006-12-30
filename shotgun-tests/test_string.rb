require File.dirname(__FILE__) + '/helper'

class TestString < RubiniusTestCase

  def test_string_to_i
    out = rp <<-CODE
    p "832".to_i
    p "-1".to_i
    CODE

    assert_equal ["832", "-1"], out
  end
  
  def test_string_substring
    out = rp <<-CODE
    puts "blah".substring(0, 2)
    puts "blah".substring(0, 4)
    puts "blah".substring(2, 2)
    CODE
    
    assert_equal ["bl", "blah", "ah"], out
  end

  def test_string_indexed
    out = rp <<-CODE
    p "blah"[0]
    p "blah"[1]
    p "blah"[2]
    CODE
    
    assert_equal ["98", "108", "97"], out
  end
  
  def test_string_prefix_eh
    out = rp <<-CODE
    p "blah".prefix?("bl")
    p "blah".prefix?("fo")
    p "go".prefix?("gogo")
    CODE
    
    assert_equal ["true", "false", "false"], out
  end
  
  def test_string_append
    out = rp <<-CODE
    b = :evan
    a = 8
    puts "\#{b} is \#{a}"
    CODE
    
    assert_equal "evan is 8", out.first
  end

  def test_string_include
    out = rp <<-CODE
    str = 'hello'
    puts str.include?(?e)
    puts str.include?('lo')
    puts str.include?('eld')
    CODE
    puts out
    assert_equal ['true', 'true', 'false'], out 
  end

  def test_string_index_fixnum
    out = rp <<-CODE
    str = 'hello'
    puts str.index(?e)
    p str.index(?w)
    p str.index(?l)
    CODE
    puts out
    assert_equal ['1', 'nil', '2'], out 
  end

  def test_string_index_string
    out = rp <<-CODE
    str = 'hello'
    puts str.index('e')
    p str.index('w')
    p str.index('llo')
    p str.index('lo')
    CODE
    puts out
    assert_equal ['1', 'nil', '2', '3'], out 
  end
  
  def test_string_slice
    out = rp <<-CODE
    str = 'hello'
    p str['zargle']
    puts str['ell']
    puts str[/ll./]
    puts str[0]
    puts str[0,1]
    CODE
    puts out
    assert_equal ['nil', 'ell', 'llo', '104', 'h'], out 
  end

  # should the to_sym call be necessary?
  def test_string_slice_dot_bug
    out = rp <<-CODE
    a = "baz.rb"
    b =   "z.rb"
    x = a.length
    y = b.length
    puts a.slice(x-y, y) == b
    puts a.slice(x-y, y).to_sym == b.to_sym
    CODE
    assert_equal ['true', 'true'], out
  end

  def test_string_split
    out = rp <<-CODE
    str = "a-b-c"
    p str.split("-")
    str = "a   b c  "
    p str.split(/\\s+/)
    CODE
    assert_equal '["a", "b", "c"]', out[0]
    assert_equal '["a", "b", "c"]', out[1]
  end
end
