require 'rubygems'
require 'test/unit'
require 'cpu/instructions'
require 'cpu/runtime'
require 'cpu/encoder'
require 'test/unit/show_code'

class TestByteArray < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_new
    ba = Rubinius::ByteArray.new(16)
    assert_equal 4, ba.fields
  end
  
  def test_store_and_fetch
    ba = Rubinius::ByteArray.new(16)
    
    ba.store_byte 14, 230
    byte = ba.fetch_byte 14
    assert_equal 230, byte
  end
end

class TestTuple < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_new
    tup = Rubinius::Tuple.new(4)
    assert_equal 4, tup.fields
  end
  
  def test_put_and_at
    tup = Rubinius::Tuple.new(4)    
    obj = Rubinius::Tuple.new(1)
    
    tup.put 0, obj
    assert_equal obj, tup.at(0)
  end
end

class TestHash < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_new
    hsh = Rubinius::Hash.new
    assert_equal CPU::Global.tuple, hsh.keys.rclass
    assert_equal CPU::Global.tuple, hsh.values.rclass
    assert hsh.bins.fixnum?
  end
  
  def test_add_and_get
    key = Rubinius::Tuple.new(2)
    val = Rubinius::Tuple.new(2)
    
    hsh = Rubinius::Hash.new
    hsh.add 1, key, val
    out = hsh.get 1
    assert_equal val, out
    assert_equal 1, hsh.entries.to_int
    
    k2 = Rubinius::Tuple.new(2)
    v2 = Rubinius::Tuple.new(2)
    
    hsh.add 17, k2, v2
    out = hsh.get 17
    assert_equal v2, out
    
    out = hsh.get 1
    assert_equal val, out
    
    out = hsh.delete 1
    assert_equal val, out
    
    out = hsh.get 17
    assert_equal v2, out
    
    hsh.add 17, k2, val
    out = hsh.delete 17
    assert_equal val, out
    
    out = hsh.delete 17
    assert out.nil?
    
    out = hsh.get 88
    assert out.nil?
  end
  
  def test_find
    key = Rubinius::String.new("name").to_sym
    val = Rubinius::Tuple.new(3)
    
    hsh = Rubinius::Hash.new
    hsh.add key.hash_int, key, val
    
    out = hsh.find key
    assert_equal val, out
  end
  
  def test_set
    key = Rubinius::String.new("name").to_sym
    val = Rubinius::Tuple.new(3)
    
    hsh = Rubinius::Hash.new
    hsh.set key, val
    
    out = hsh.find key
    assert_equal val, out
  end
  
  
  def test_delete
    key = Rubinius::Tuple.new(2)
    val = Rubinius::Tuple.new(2)
    
    hsh = Rubinius::Hash.new
    hsh.add 1, key, val
    
    out = hsh.delete 1
    assert_equal val, out
    
    out = hsh.delete 1
    assert out.nil?
  end
  
  def test_find_entry
    key = Rubinius::Tuple.new(2)
    val = Rubinius::Tuple.new(2)
    
    hsh = Rubinius::Hash.new
    out = hsh.find_entry(1)
    assert out.nil?
    
    hsh.add 1, key, val
    out = hsh.find_entry(1)
    
    assert_equal 1, out.at(0).to_int
    assert_equal key, out.at(1)
    assert_equal val, out.at(2)
    assert out.at(3).nil?
    
    hsh.add 17, key, val
    
    o2 = hsh.find_entry(17)
    
    assert_equal o2, out.at(3)
  end
  
  def test_s_new_from_tuple
    tup = Rubinius::Tuple.new(4)
    sym1 = Rubinius::String.new("name").to_sym
    sym2 = Rubinius::String.new("age").to_sym
    obj1 = RObject.wrap(79)
    obj2 = RObject.wrap(23)
    
    tup.put 0, sym1
    tup.put 1, obj1
    tup.put 2, sym2
    tup.put 3, obj2
    
    hsh = Rubinius::Hash.new_from_tuple tup
    assert_equal obj1, hsh.get(sym1.hash_int)
    assert_equal obj2, hsh.get(sym2.hash_int)
  end
end

class TestString < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_new
    str = Rubinius::String.new("blah")
    assert_equal 4, str.bytes.to_int
    assert_equal 4, str.characters.to_int
    assert str.encoding.nil?
    
    assert_equal "blah", str.as_string
  end
  
  def test_hash_int
    str = Rubinius::String.new("aoeuaoeuaoeuaoeuaoeuaoeuaoeu")
    int = str.hash_int
    assert int <= 0x1fffffff
    
    s1 = Rubinius::String.new("a")
    s2 = Rubinius::String.new("a")
    
    assert_equal s1.hash_int, s2.hash_int
    
    s1 = Rubinius::String.new("b")
    
    assert s1.hash_int != s2.hash_int
  end
  
  def test_to_sym
    str = Rubinius::String.new("Blah")
    sym = str.to_sym
    assert sym.symbol?
  end
end

class TestSymbolTable < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_lookup
    tbl = Rubinius::SymbolTable.new
    str = Rubinius::String.new("Class")
    
    sym = tbl.lookup(str)
    
    assert sym.symbol?
    assert_equal 0, sym.symbol_index
    
    sym2 = tbl.lookup(str)
    
    assert_equal sym.symbol_index, sym2.symbol_index
    
    str2 = Rubinius::String.new("Module")
    
    sym3 = tbl.lookup(str2)
    
    assert_equal 1, sym3.symbol_index
  end
  
  def test_find_string
    tbl = Rubinius::SymbolTable.new
    str = Rubinius::String.new("Class")

    sym = tbl.lookup(str)
    str2 = tbl.find_string(sym)
    
    assert_equal str.as_string, str2.as_string
  end
end

class TestSymbol < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_string
    str = Rubinius::String.new("Class")
    sym = CPU::Global.symbols.lookup(str)
    
    str2 = sym.string
    assert_equal str.as_string, str2.as_string
    
    assert_equal ":Class", sym.as_string
  end
end


