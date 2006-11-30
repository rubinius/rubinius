require 'cpu/runtime'

class Machine
  
  def initialize
    @cpu = CPU.new
    @cpu.bootstrap
    @constructor = @cpu.constructor
    @error = false
  end
  
  attr_reader :cpu, :error
  
  def delete
    @cpu.delete
  end
  
  def run_file(path)
    # TODO SydneyParser needs to have a way to return
    # and error without raising a CRuby exception so that
    # we can easily see that error here and then raise a
    # Rubinius exception.
    begin
      meth = @constructor.load_file path
    rescue LoadError => e
      puts "Unable to run '#{path}'. #{e.message}."
      return false
    end
    
    name = Rubinius::String.new("__script__").to_sym
    
    @cpu.goto_method @cpu.main, meth, 0, name
    @cpu.run
    handle_exception
    return true
  end
  
  def compile_file(path, cached=true)
    begin
      meth = @constructor.load_file path, cached
    rescue LoadError => e
      puts "Unable to compile '#{path}'. #{e.message} (#{e.class})"
    end
  end
  
  def run_code(code)
    begin
      meth = @constructor.compile(code)
    rescue LoadError => e
      puts "Unable to evaluate code. #{e.message}"
      return false
    end
    
    name = Rubinius::String.new("__eval__").to_sym
    
    @cpu.goto_method @cpu.main, meth, 0, name
    @cpu.run
    handle_exception
    return true
  end
  
  def handle_exception    
    exc = @cpu.exception
    return if exc.nil?
    
    unless exc.reference?
      puts "Fuck. wtf is this exception! #{exc}"
      return
    end
        
    msg = exc.at(0)
    unless msg.reference?
      puts "WARNING! Exception of type #{exc.rclass.name.as(:symbol).as_string} with nil message!\n"
      return
    end
    msg.as :string
    kind = exc.rclass.name.as(:symbol).string.as_string
    puts "Exception detected: #{msg.as_string} (#{kind})"
    @error = true
  end
  
  def set_const(str, val)
    con = CPU::Global.object.constants
    con.as :hash
    sym = Rubinius::String.new(str).to_sym
    con.set sym, val
  end
  
  def import_args(args)
    argv = Rubinius::Array.new(args.size)
    i = 0
    args.each do |a|
      str = Rubinius::String.new(a.to_s)
      argv.set i, str
      i += 1
    end
    
    set_const "ARGV", argv
  end
  
  def setup_standard_io
    set_const "STDIN", Rubinius::IO.new(0)
    set_const "STDOUT", Rubinius::IO.new(1)
    set_const "STDERR", Rubinius::IO.new(2)
  end
end