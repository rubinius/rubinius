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
end