require 'test/unit'
require 'sexp/simple_processor'


class TSP < SimpleSexpProcessor
  def process_name(x)
    x.shift
    return x.shift
  end
  
  def process_top(x)
    x.shift
    out = ""
    while e = x.shift
      out << process(e)
      out << " "
    end
    return out
  end
  
  def process_all(x)
    x.shift
    return concat_all("", x)
  end
  
  def process_array(x)
    x.shift
    []
  end
  
  def process_string(x)
    x.shift
    ""
  end
  
  def process_if(x)
    x.shift
    [:if, process(x.shift), process(x.shift)]
  end
end

class TestSimpleSexpProcessor < Test::Unit::TestCase
  def test_process_defined_method
    pr = TSP.new
    out = pr.process([:name, "evan"])
    assert_equal "evan", out
  end
  
  def test_process_multiple
    pr = TSP.new
    out = pr.process([:top, [:name, "bob"], [:name, "alice"]])
    assert_equal "bob alice ", out
  end
  
  
  def test_process_all
    pr = TSP.new
    out = pr.process([:all, [:name, "bob"], [:name, "alice"], [:name, "jones"]])
    assert_equal "bobalicejones", out
  end
  
  def test_process_unknown
    pr = TSP.new
    pr.default_object = []
    out = pr.process([:unknown, [:name, "bob"], [:name, "alice"], [:name, "jones"]])
    assert_equal [:unknown, "bob", "alice", "jones"], out
  end
  
  def test_any_return
    pr = TSP.new
    pr.ignore_return!
    out = nil
    assert_nothing_raised do
      out = pr.process([:any, [:array], [:string]])
    end
    
  end
  
  def test_default_with_expanding
    pr = TSP.new
    pr.default_object = []
    out = pr.process( [:if, [:true], [:call, 1]] )
  end
  
end