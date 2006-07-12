require 'translation/infrastructure'
require 'test/unit'

class TestRsClassBody < Test::Unit::TestCase
  
  def setup
    @st = RsStructure.new
    @cl = ClassInfo.new(:Blah)
    @cb = RsClassBody.create @cl, @st
  end
  
  def test_process_fcall
    assert_nothing_raised do
      @cb.process [:fcall, :attr_accessor, [:array, [:lit, :x]]]
    end
    
    assert_raises(RuntimeError) do
      @cb.process [:fcall, :blah, [:array, [:lit, :x]]]
    end
  end
  
  def test_process_defn
    input = [:defn, :go, [:scope, [:block,
      [:args],
      [:true]
    ]]]
    
    assert @cl.methods.size == 0
    @cb.process input
    assert_kind_of String, @cl.methods[:go]
    ft = @st.functions.find(Type.Blah, :go)
    assert_equal Type.bool, ft.ret_type
  end
end

=begin
class TestInfrastructure < Test::Unit::TestCase
  def test_process_class(x)
    
  end
end
=end