require 'rubygems'
require 'bytecode/constructor'
require 'cpu/runtime'
require 'test/unit'
require 'tempfile'

class TestConverter < Test::Unit::TestCase
  def setup
    @cpu = CPU.new
    @cpu.bootstrap
    @con = Bytecode::Constructor.new(@cpu)
  end
  
  def teardown
    @cpu.delete
  end
  
  def test_convert_to_sexp
    code = "true and false"
    expected = [:newline, 1, "(eval)", [:and, [:true], [:false]]]
    assert_equal expected, @con.convert_to_sexp(code)  
  end
  
  def test_compile
    code = "true and false"
    out = @con.compile code
    assert_kind_of RObject, out
    assert_equal CPU::Global.cmethod, out.rclass
  end
  
  def test_compile_file
    tmp = Tempfile.new("rubinius")
    tmp << "true and false\n"
    tmp.close
    
    out = @con.compile_file tmp.path
    assert_kind_of RObject, out
    assert_equal CPU::Global.cmethod, out.rclass
  end
  
  def test_compiled_path
    path = "test.rb"
    out = @con.compiled_path(path)
    assert_equal "test.rbc", out
    
    path = "test.blah"
    out = @con.compiled_path(path)
    assert_equal "test.blah.rbc", out
  end
  
  def test_file_newer
    assert !@con.file_newer?("a", "b")
    t = Tempfile.new("blah")
    t.close
    assert !@con.file_newer?("a", t.path)
    t2 = Tempfile.new("blah")
    t2.close
    assert @con.file_newer?(t2.path, t.path)
    sleep 1
    t3 = Tempfile.new("blah")
    t3.close
    assert @con.file_newer?(t3.path, t.path)
    t.close!
    assert !File.exists?(t.path)
    assert @con.file_newer?(t3.path, t.path)
  end
  
  # Load file automatically caches the cmethod that is
  # created.
  def test_load_file
    cpath = nil
    dir = File.dirname(__FILE__)
    path = "#{dir}/load_test.rb"
    File.open(path, "w") do |fd|
      fd << "true and false\n"
    end
    
    out = @con.load_file path
    assert_kind_of RObject, out
    assert_equal CPU::Global.cmethod, out.rclass
    
    cpath = "#{dir}/load_test.rbc"
    assert File.exists?(cpath)
    
    File.unlink path
    out = @con.load_file path
    assert_kind_of RObject, out
    assert_equal CPU::Global.cmethod, out.rclass
  ensure
    File.unlink(cpath) if File.exists?(cpath)
  end
  
end