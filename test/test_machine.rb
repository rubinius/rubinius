require 'test/unit'
require 'tempfile'
require 'machine'

class TestMachine < Test::Unit::TestCase
  def setup
    @machine = Machine.new
  end
  
  def teardown
    @machine.delete
  end
  
  def get_const(name)
    sym = Rubinius::String.new(name).to_sym
    con = CPU::Global.object.constants
    con.as :hash
    con.find(sym)
  end

  def test_run_file
    tmp = Tempfile.new("rb")
    tmp << "Blah = 98"
    tmp.close
    
    @machine.run_file tmp.path
    con = CPU::Global.object.constants
    con.as :hash
    
    sym = Rubinius::String.new("Blah").to_sym
    
    out = con.find(sym)
    assert_equal 98, out.to_int
  end
  
  def test_run_code
    code = "Blah = 98"
    
    @machine.run_code code
    con = CPU::Global.object.constants
    con.as :hash
    
    sym = Rubinius::String.new("Blah").to_sym
    
    out = con.find(sym)
    assert_equal 98, out.to_int
  end
  
  def test_import_args
    args = ["-e", "99", "blah"]
    @machine.import_args args
    argv = get_const("ARGV")
    assert argv.reference?
    assert_equal argv.rclass, CPU::Global.array
    argv.as :array
    assert_equal 3, argv.total.to_int
  end
  
  def test_setup_standard_io
    @machine.setup_standard_io
    
    stdout = get_const("STDOUT")
    assert_equal CPU::Global.io, stdout.rclass
    stdout.as :io
    assert_equal 1, stdout.descriptor.to_int
    
    stdin = get_const("STDIN")
    assert_equal CPU::Global.io, stdin.rclass
    stdin.as :io
    assert_equal 0, stdin.descriptor.to_int
    
    stderr = get_const("STDERR")
    assert_equal CPU::Global.io, stderr.rclass
    stderr.as :io
    assert_equal 2, stderr.descriptor.to_int
    
  end
  
end