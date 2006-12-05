require 'rubygems'
require 'test/unit'
require 'test/unit/show_code'
require 'shotgun-tests/helper'

unless File.exists?("code-cache")
  Dir.mkdir "code-cache"
end

class TestSexp < Test::Unit::TestCase

  include RubiniusHelper
  
  def to_sexp(what)
    code = "'#{what}'.to_sexp('test',1,false)"
    out = run_code("p #{code}\n", 2).strip
    return out
  end
  
  def test_number
    assert_equal '[:lit, 834234]', to_sexp("834234")
  end
  
  def test_regex
    assert_equal '[:regex, "blah"]', to_sexp("/blah/")
  end
  
  def test_string
    assert_equal '[:str, "hello"]', to_sexp("\"hello\"")
  end
  
  def test_lvar
    exc = '[:block, [:lasgn, :a, [:lit, 1]], [:lvar, :a]]'
    assert_equal exc, to_sexp("a = 1; a")
  end
  
  def test_ivar
    assert_equal '[:ivar, :@blah]', to_sexp("@blah")
  end
  
  def test_iasgn
    assert_equal '[:iasgn, :@blah, [:lit, 1]]', to_sexp("@blah = 1")
  end
  
  def test_gvar
    assert_equal '[:gvar, :$blah]', to_sexp("$blah")
  end
  
  def test_gasgn
    assert_equal '[:gasgn, :$blah, [:lit, 1]]', to_sexp("$blah = 1")
  end
  
  def test_symbol
    assert_equal '[:lit, :blah]', to_sexp(":blah")
  end
  
  def test_expand
    exc = '[:block, [:lasgn, :a, [:lit, 1]], [:dstr, "hello ", [:evstr, [:lvar, :a]], [:str, ", you rock."]]]'
    assert_equal exc, to_sexp('a = 1; "hello #{a}, you rock."')
  end
  
  def test_def
    input = "def name; 1; end"
    exc = '[:defn, :name, [:scope, [:block, [:args], [:lit, 1]], []]]'
    assert_equal exc, to_sexp(input)
  end
  
  def test_def_args
    input = "def name(a, b); 1; end"
    exc = '[:defn, :name, [:scope, [:block, [:args, [:a, :b], [], nil, nil], [:lit, 1]], [:a, :b]]]'
    assert_equal exc, to_sexp(input)
  end
  
  def test_class
    input = "class Blah < A::B; end"
    exc = '[:class, [:colon2, :Blah], [:colon2, [:const, :A], :B], [:scope, []]]'
    assert_equal exc, to_sexp(input)
  end
  
  def test_heredoc
    input = 'a = <<BLAH
hello
BLAH
'
    exc = "[:lasgn, :a, [:str, \"hello\n\"]]"
    assert_equal exc, to_sexp(input)
  end
end
