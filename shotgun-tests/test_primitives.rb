require 'rubygems'
require_gem 'ZenTest', ">= 3.4.0"
require 'test/unit'
require 'test/unit/show_code'

class RubiniusError < RuntimeError; end

class TestPrimitives < Test::Unit::TestCase
  
  def caller_name(which=1)
    line = caller[which]
    parts = line.split(/\s+/)
    ln = parts.first.split(":")[-2]
    parts.last[1..-2]
  end
  
  def run_code(code, name=1, say=nil)
    name = caller_name(name) + "-#{code.hash}"
    path = "code-cache/#{name}.rb"
    if !File.exists?(path) or (code != File.read(path))    
      File.open(path, "w") do |fd|
        fd << code
      end
      system("./bin/rcompile code-cache/#{name}.rb")
    end
    r, w = IO.pipe
    r2, w2 = IO.pipe
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      exec "./shotgun/rubinius lib/kernel.rbc code-cache/#{name}.rbc"
    }
    r2.close
    w.close
    if say
      w2 << say
    end
    w2.close
    Process.wait(pid)
    if $?.exitstatus != 0
      error = r.read
      error << "\n    ========================================="
      raise RubiniusError, error
    end
    out = r.read
    r.close
    return out
    # io = IO.popen()
    # io.read
  end
  
  def rp(code)
    out = run_code(code, 2)
    out.strip.split("\n")
  end
  
  def ol(code)
    run_code("p #{code}\n", 2).strip
  end
  
  def test_add
    assert_equal "3", ol("1 + 2")
  end
  
  def test_sub
    assert_equal "5", ol("10 - 5")
  end
  
  def test_equal
    assert_equal "true", ol("5 == 5")
    assert_equal "false", ol("5 == 6")
  end
    
  def test_compare
    assert_equal "-1", ol("3 <=> 4")
    assert_equal "0", ol("3 <=> 3")
    assert_equal "1", ol("4 <=> 3")
  end
  
  def test_at
    out = run_code <<-CODE
    tp = Tuple.new(5)
    tp.put 3, 88
    p tp.at(3)
    CODE
    
    assert_equal "88", out.strip
  end
  
  def test_put
    # test_at tests both.
  end
  
  def test_fields
    out = run_code <<-CODE
    tp = Tuple.new(5)
    p tp.fields
    CODE
    
    assert_equal "5", out.strip
  end
  
  def test_allocate
    out = rp <<-CODE
    ary = Array.allocate
    p(ary.fields == Array.instance_fields)
    p(ary.class)
    CODE
    
    assert_equal ["true", "Array"], out
  end
  
  def test_allocate_count
    out = rp <<-CODE
    tp = Tuple.new(99)
    p(tp.fields)
    CODE
    
    assert_equal ["99"], out
  end
  
  def test_allocate_bytes
    out = rp <<-CODE
    ba = ByteArray.new(16)
    p(ba.fields)
    CODE
    
    assert_equal ["4"], out
  end
  
  def test_create_block
    out = rp <<-CODE
    def blah
      yield
    end
    
    blah { puts "works" }
    CODE
    
    assert_equal ["works"], out
  end
  
  def test_block_given
    out = rp <<-CODE
    def blah
      block_given?
    end
    
    out = blah() { true }
    p out
    out = blah()
    p out
    CODE
    
    assert_equal ["true", "false"], out
  end
  
  def test_block_call
    # test_block_given tests that block_call works.
  end
  
  def test_io_write
    out = rp <<-CODE
    STDOUT.write "hello evan\\n"
    CODE
    
    assert_equal ["hello evan"], out
  end
  
  def test_io_read
    code = <<-CODE
    puts STDIN.read(5)
    CODE
    
    say = "hello"
    out = run_code(code, 1, say)
    assert_equal say, out.strip
  end
  
  def test_create_pipe
    out = rp <<-CODE
    r, w = IO.pipe
    w.puts "test_create_pipe\\n"
    puts r.read(16)
    CODE
    
    assert_equal ["test_create_pipe"], out
  end
  
  def test_io_open
    File.open("code-cache/tmp_io_open", "w") do |fd|
      fd << "test 1234\n"
    end
    
    out = rp <<-CODE
    fd = File.new("code-cache/tmp_io_open","r")
    puts fd.read(9)
    fd.close
    CODE
    
    assert_equal ["test 1234"], out
  end
  
  def test_io_close
    File.open("code-cache/tmp_io_open", "w") do |fd|
      fd << "test 1234\n"
    end
    
    out = rp <<-CODE
    fd = File.new("code-cache/tmp_io_open","r")
    p fd.closed?
    fd.close
    p fd.closed?
    begin
      fd.close
      puts "worked"
    rescue Object => e
      p e.class
    end
    CODE
    
    assert_equal ["false", "true", "IOError"], out
  end
  
  def test_gettimeofday
    i = Time.now.to_i
    out = rp <<-CODE
    t = Time.now
    p(t.to_i > 1000000)
    CODE
    assert_equal ["true"], out
  end
  
  def test_strftime
    out = rp <<-CODE
    t = Time.now
    puts t
    CODE
    assert_equal Time.now.to_s, out.first.to_s
  end
  
  def test_fixnum_to_s
    i = 54352344
    out = rp <<-CODE
    puts #{i}.to_s(18)
    CODE
    
    assert_equal i.to_s(18), out.first
  end
  
  def test_logical_class
    out = rp <<-CODE
    p 10.class
    p :blah.class
    p [].class
    CODE
    
    assert_equal ["Fixnum", "Symbol", "Array"], out
  end
  
  def test_object_id
    out = rp <<-CODE
    p 10.object_id
    p :blah.object_id
    p [].object_id
    CODE
    
    out.map { |i| i.to_i }.each do |e|
      assert e > 0
    end
  end
  
  def test_hash_object
    
  end
end
