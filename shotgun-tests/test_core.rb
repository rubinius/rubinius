require 'shotgun-tests/helper'

class TestCore < Test::Unit::TestCase
  include RubiniusHelper
  
  def test_at_exit
    out = rp <<-CODE
    at_exit { puts "exited" }
    CODE
    
    assert_equal ["exited"], out
  end

  def test_class_new
    out = rp <<-CODE
    class Blah
    end
    
    b = Blah.new
    p b.class
    CODE
    
    assert_equal ["Blah"], out
  end
  
  def test_closest_instance_methods
    out = rp <<-CODE
    class Blah
      def go
        nil
      end
    end
    
    p Blah.instance_methods(false)
    CODE
    
    assert_equal '["go"]', out.first
  end
  
  def test_array_each
    out = rp <<-CODE
    ary = [1,2,3]
    ary.each do |i|
      p i
    end
    CODE
    
    assert_equal ['1','2','3'], out
  end
  
  def test_string_prefix_eh
    out = rp <<-CODE
    p "blah".prefix?("bl")
    p "blah".prefix?("fo")
    p "go".prefix?("gogo")
    CODE
    
    assert_equal ["true", "false", "false"], out
  end
  
  def test_exceptions
    out = rp <<-CODE
    begin
      raise ArgumentError, "you want more!"
    rescue Exception => e
      puts e.message
    end
    CODE
    
    assert_equal ["you want more!"], out
  end

  def test_ensure
    out = rp <<-CODE
    begin
      raise ArgumentError, "you want more!"
    rescue Exception => e
      puts e.message
    ensure
      puts 'ensure'
    end
    CODE
    
    assert_equal ["you want more!", "ensure"], out
  end

  def test_retry
    out = rp <<-CODE
    @count = 1
    begin
      p @count
      raise ArgumentError, 'just kidding' unless @count > 3
    rescue Exception => e
      @count += 1
      retry
    else
      p 7
    ensure
      p @count
    end
    CODE

    assert_equal %w(1 2 3 4 7 4), out
  end
  
  def test_send
    out = rp <<-CODE
    class Blah
      def go
        puts "hello evan"
      end
    end
    b = Blah.new
    b.send :go
    CODE
    
    assert_equal ["hello evan"], out
  end
  
  def test_normal_method
    out = rp <<-CODE
    class Blah
      def go
        puts "hello evan"
      end
    end
    
    Blah.new.go
    CODE
    
    assert_equal ["hello evan"], out
  end
  
  def test_ternary
    out = rp <<-CODE
    a = true ?  "works" : "broken"
    b = false ? "broken" : "works"
    puts a
    puts b
    CODE
    
    assert_equal ["works", "works"], out
  end
  
  def test_triple_equal
    out = rp <<-CODE
    p(Symbol === :blah)
    p(Class === :blah)
    CODE
    
    assert_equal ["true", "false"], out
  end
  
  def test_multiple_print
    out = rp <<-CODE
    print "hello ", "evan"
    CODE

    assert_equal "hello evan", out.first
  end
  
  def test_string_append
    out = rp <<-CODE
    b = :evan
    a = 8
    puts "\#{b} is \#{a}"
    CODE
    
    assert_equal "evan is 8", out.first
  end
  
  def test_inherited
    out = rp <<-CODE
    class Blah
      def self.inherited(sub)
        puts "\#{self} was inherited"
      end
    end
    
    class A < Blah
    end
    CODE
    
    assert_equal "Blah was inherited", out.first
  end
  
  def test_constant_access
    out = rp <<-CODE
    class Blah
      List = []
      
      def self.add(a)
        List << a
      end
    end
    
    Blah.add(8)
    p Blah::List
    CODE
    
    assert_equal ["[8]"], out
  end
  
  def test_empty_method
    out = rp <<-CODE
    class Blah
      def blah; end
    end
    
    p Blah.new.blah
    CODE
    
    assert_equal "nil", out.first
  end
  
  def test_method_with_optional
    out = rp <<-CODE
    class Blah
      def go(name="evan")
        puts "hello \#{name}"
      end
    end
    
    b = Blah.new
    b.go
    b.go("ryan")
    CODE
    
    assert_equal ["hello evan", "hello ryan"], out
  end
  
  def test_post_unless
    out = rp <<-CODE
    a = "blah"
    a = "hello" unless false
    puts a
    a = "blah" unless true
    puts a
    CODE
    
    assert_equal ["hello", "hello"], out
  end
  
  def test_pipe_or
    out = rp <<-CODE
    a = "true" || true
    puts a
    a = nil || "true"
    puts a
    CODE
    
    assert_equal ["true", "true"], out
  end
  
  def test_double_equal
    out = rp <<-CODE
    p true == true
    p false == true
    p 1 == 1
    p 2 == 1
    p "blah" == "blah"
    p "bleh" == "blah"
    CODE
    
    assert_equal ["true", "false", "true", "false", "true", "false"], out
  end
  
  def test_triple_equal_on_class
    out = rp <<-CODE
    p Class === Symbol
    p Symbol === 1
    CODE
    
    assert_equal ["true", "false"], out
  end
  
  def test_instance_of_eh
    out = rp <<-CODE
    p :blah.instance_of?(Object)
    p :blah.instance_of?(Symbol)
    CODE
    
    assert_equal ["false", "true"], out
  end
  
  def test_nil_eh
    out = rp <<-CODE
    p 1.nil?
    p nil.nil?
    CODE
    
    assert_equal ["false", "true"], out
  end
  
  def test_bang_equal
    out = rp <<-CODE
    p(1 != 1)
    p(1 != 2)
    p(:blah != :blah)
    p(:bleh != :blah)
    p("blah" != "blah")
    CODE
    
    assert_equal ["false", "true", "false", "true", "false"], out
  end
  
  def test_equal_eh
    out = rp <<-CODE
    a = "blah"
    p(a.equal?("blah"))
    p(a.equal?(a))
    CODE
    
    assert_equal ["false", "true"], out
  end
  
  def test_require
    File.open("code-cache/test_require.rb", "w") do |fd|
      fd.puts "puts 'hello world'"
    end
    
    `./bin/rcompile code-cache/test_require.rb`
    
    out = rp <<-CODE
    require 'code-cache/test_require'
    CODE
    
    assert_equal "hello world", out.first
    
    out = rp <<-CODE
    begin
      require 'not_there'
    rescue LoadError
      puts "works"
    end
    CODE
    
    assert_equal "works", out.first
  end
  
  def test_block_semantics
    out = rp <<-CODE
    def blah(&bl)
      return bl
    end
    
    pr = blah() { |a, b| a + b }
    p pr.call(8, 19)
    
    l = lambda() { 8 + 8 }
    p l.call
    CODE
    
    assert_equal ["27", "16"], out
  end
  
  def test_callcc
    out = rp <<-CODE
    def blah
      puts "making"
      save = nil
      callcc do |cc|
        save = cc
        return cc
      end
      8 + 8
      puts "back in method"
      return save
    end
    i = 0
    puts "before"
    cc = blah()
    i += 1
    puts "before call"
    cc.call if i < 5
    puts "after"
    CODE
    exc = ["before", "making", "before call", "back in method", "before call",
           "back in method", "before call", "back in method", "before call",
           "back in method", "before call", "after"]
    assert_equal exc, out
  end
  
  def test_stack_explosion
    e = nil
    begin
      out = rp <<-CODE
      def go
        true
      end
    
      def blah
        i = 0
        while i < 5
          go()
        end
      end
    
      blah()
      CODE
    rescue RubiniusError => e
      assert_match(/StackExploded/, e.message)
    end
    
    assert(e)
  end
  
  def test_super
    out = rp <<-CODE
    
    class Top
      def initialize
        puts "top"
      end
    end
    
    class Go < Top
      def initialize
        puts "here"
        super()
      end
    end
    
    class Blah < Go
      def initialize
        puts "new blah"
        super()
      end
    end
    
    Blah.new
    CODE
    
    assert_equal ["new blah", "here", "top"], out
  end

  def test_splat
    out = rp <<-CODE
    p *5
    p *[6]
    p *%w(1 2 3)
    x, *y = [8,9,10,11]
    p x
    p y
    CODE
    assert_equal ['5', '6', '"1"', '"2"', '"3"', '8', '[9, 10, 11]'], out
  end

  def test_throw_catch
    out = rp <<-CODE
      def one_two
        [1,2,3].each do |i|
          p i
          throw(:crazy) if i == 2
        end
      end

      catch(:crazy) do
        one_two
        p 7
      end
    CODE
    assert_equal %w(1 2), out
  end

  def test_uncaught_throw
    out = rp <<-CODE
      [1,2,3].each do |i|
        p i
        throw(:crazy) if i == 2
      end
    CODE
    assert_match /uncaught/, out.join
  end

  def test_match3
    out = rp <<-CODE
      p "abc" =~ /c/
    CODE
    assert_equal ['2'], out
  end

  def test_match3_no_match
    out = rp <<-CODE
      p "abc" =~ /z/
    CODE
    assert_equal ['nil'], out
  end

  def test_match3_inverted
    out = rp <<-CODE
      p "xyz" !~ /r/
      p "xyz" !~ /y/
    CODE
    assert_equal ['true', 'false'], out
  end
end
