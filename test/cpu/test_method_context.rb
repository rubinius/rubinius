require File.expand_path(File.dirname(__FILE__) + '/../test_helper')
require 'cpu/runtime'
require 'types'

class TestMethodContext < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    Rubinius.cpu = @cpu
    @cpu.bootstrap
  end
  
  def test_new_anonymous
    meth = Rubinius::MethodContext.new_anonymous
    assert_equal 0, meth.ip.to_int
    assert_equal 0, meth.sp.to_int
  end
  
  def test_new
    meth = Rubinius::MethodContext.new(3)
    assert_equal 5, meth.locals.fields
  end
end
