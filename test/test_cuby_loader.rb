require 'cuby'
require 'cuby_loader'
require 'test/unit'

class TestCubyLoader < Test::Unit::TestCase
  def setup
    @c = Cuby.new
    @l = Cuby::Loader.new(@c)  
  end
  
  def test_from_string
    @c.operators << :+
    @l.load_string "1 + 2"
    assert_equal "1 + 2", @c.code
    
    assert_raises(Cuby::UnknownMethod) do
      @l.load_string "1 - 2"
    end
  end
end