require 'rubygems'
require 'cpu/simple_marshal'
require 'test/unit'

require 'cpu/runtime'

class TestSimpleMarshal < Test::Unit::TestCase
  
  def setup
    @sm = SimpleMarshal.new
    @cpu = CPU.new
    @cpu.bootstrap
  end

  def teardown
    @cpu.delete
  end
    
  def marshal(*code)
    @sm.marshal *code
  end
  
  def unmarshal(str)
    @sm.unmarshal str
  end
  
  def test_int
    int = RObject.wrap(932)
    out = marshal int
    assert_equal 5, out.size
    assert_equal ?i, out[0]
    intv = out[1..-1].unpack("N").first
    assert_equal intv, 932
    
    obj = unmarshal out
    assert_equal int, obj
  end
  
  def test_str
    obj = Rubinius::String.new("blahbleh")
    mar = marshal obj
    assert_equal 13, mar.size
    assert_equal ?s, mar[0]
    sz = mar[1,4].unpack("N").first
    assert_equal 8, sz
    assert_equal "blahbleh", mar[5..-1]
    
    obj2 = unmarshal mar
    assert_equal obj.as_string, obj2.as_string
  end
  
  def test_sym
    obj = Rubinius::String.new("blahbleh").to_sym
    mar = marshal obj
    assert_equal 13, mar.size
    assert_equal ?x, mar[0]
    sz = mar[1,4].unpack("N").first
    assert_equal 8, sz
    assert_equal "blahbleh", mar[5..-1]
    
    obj2 = unmarshal mar
    assert_equal obj, obj2
  end
  
  def test_tuple
    tup = Rubinius::Tuple.new(3)
    tup.put 0, RObject.wrap(32)
    tup.put 1, RObject.wrap(111122)
    
    sym = Rubinius::String.new("blahbleh").to_sym
    tup.put 2, sym
    
    mar = marshal tup
    assert_equal ?p, mar[0]
    sz = mar[1,4].unpack("N").first
    assert_equal 3, sz
    
    tup2 = unmarshal mar
    assert_equal 32, tup2.at(0).to_int
    assert_equal 111122, tup2.at(1).to_int
    assert_equal sym, tup2.at(2)
  end
  
  def test_bytes
    obj = Rubinius::ByteArray.from_string("blahbleh")
    mar = marshal obj
    assert_equal 13, mar.size
    assert_equal ?b, mar[0]
    sz = mar[1,4].unpack("N").first
    assert_equal 8, sz
    assert_equal "blahbleh", mar[5..-1]
    
    obj2 = unmarshal mar
    assert_equal obj.as_string, obj2.as_string
  end
  
  def test_method
    codes = "\2\1"
    lcls = 4
    cm = Rubinius::CompiledMethod.from_string(codes, lcls)
    mar = marshal cm
    assert_equal ?m, mar[0]
    sz = mar[1,4].unpack("N").first
    assert_equal cm.fields, sz
    
    cm2 = unmarshal mar
    assert_equal cm2.fields, cm.fields
    fel = cm.fields
    cm2.as :cmethod
    cm.as :cmethod
    #Fields = [:bytecodes, :primitive, :locals, :literals,
    #  :arguments, :exceptions]
    cm.as :cmethod
    ba1 = cm.bytecodes
    ba1.as :bytearray
    # 0.upto(fel-1) { |x| p cm2.at(x) }
    cm2.as :cmethod
    ba2 = cm2.bytecodes
    ba2.as :bytearray
    assert !ba2.nil?
    assert !ba1.nil?
    assert_equal ba1.as_string, ba2.as_string
    assert_equal cm.primitive, cm2.primitive
    assert_equal cm.locals, cm2.locals
    assert_tuple_equal cm.literals, cm2.literals
    assert_tuple_equal cm.arguments, cm2.arguments
  end
  
  def assert_tuple_equal(t1, t2)
    assert_equal t1.fields, t2.fields
    sz = t1.fields
    0.upto(sz-1) do |idx|
      assert_equal t1.at(idx), t2.at(idx)
    end
  end
end
