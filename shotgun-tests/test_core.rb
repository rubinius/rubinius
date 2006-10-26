require 'rubygems'
require 'test/unit'
require 'test/unit/show_code'
require 'shotgun-tests/helper'

class TestCore < Test::Unit::TestCase
  include RubiniusHelper
  
  def test_at_exit
    out = rp <<-CODE
    at_exit { puts "exited" }
    CODE
    
    assert_equal ["exited"], out
  end
  
  def test_class_new
    out = rp <<-CODE
    class Blah
    end
    
    b = Blah.new
    p b.class
    CODE
    
    assert_equal ["Blah"], out
  end
  
  def test_closest_instance_methods
    out = rp <<-CODE
    class Blah
      def go
        nil
      end
    end
    
    p Blah.instance_methods(false)
    CODE
    
    assert_equal '["go"]', out.first
  end
  
  def test_array_each
    out = rp <<-CODE
    ary = [1,2,3]
    ary.each do |i|
      p i
    end
    CODE
    
    assert_equal ['1','2','3'], out
  end
  
  def test_string_prefix_eh
    out = rp <<-CODE
    p "blah".prefix?("bl")
    p "blah".prefix?("fo")
    p "go".prefix?("gogo")
    CODE
    
    assert_equal ["true", "false", "false"], out
  end
end