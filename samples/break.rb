require 'test/unit'

class TestControl < Test::Unit::TestCase
  def test_if_break
    begin
      if true
        break
      end
    rescue Object => e
      assert_kind_of LocalJumpError, e
    end
  end
  
  def test_if_next
    begin
      if true
        next
      end
    rescue Object => e
      assert_kind_of LocalJumpError, e
    end
  end
  
  def test_if_redo
    begin
      if true
        redo
      end
    rescue Object => e
      assert_kind_of LocalJumpError, e
    end
  end
  
  def test_if_retry
    begin
      if true
        retry
      end
    rescue Object => e
      assert_kind_of LocalJumpError, e
    end
  end
  
  def test_while_break
    while true
      break
    end
  end
end
