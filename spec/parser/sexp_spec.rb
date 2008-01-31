require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/sexp_expectations'

class String
  alias :old_to_sexp :to_sexp
  def to_sexp
    old_to_sexp('test', 1, false)
  end
end

describe "Shotgun" do
  it "converts a number to an sexp" do
    "834234".to_sexp.should == [:lit, 834234]
  end

  it "converts a regexp to an sexp" do
    "/blah/".to_sexp.should == [:regex, "blah", 0]
    "/blah/i".to_sexp.should == [:regex, "blah", 1]
    "/blah/u".to_sexp.should == [:regex, "blah", 64]
  end

  it "converts a string to an sexp" do
    "\"hello\"".to_sexp.should == [:str, "hello"]
  end

  it "converts a local var to an sexp" do
    "a = 1; a".to_sexp.should == [:block, [:lasgn, :a, [:lit, 1]], [:lvar, :a, 0]]
  end

  it "converts an instance variable to an sexp" do
    "@blah".to_sexp.should == [:ivar, :@blah]
  end

  it "converts an instance variable assignment to an sexp" do
    "@blah = 1".to_sexp.should == [:iasgn, :@blah, [:lit, 1]]
  end

  it "converts a global variable to an sexp" do
    "$blah".to_sexp.should == [:gvar, :$blah]
  end

  it "converts a global variable assignment to an sexp" do
    "$blah = 1".to_sexp.should == [:gasgn, :$blah, [:lit, 1]]
  end

  it "converts a symbol to an sexp" do
    ":blah".to_sexp.should == [:lit, :blah]
  end

  it "converts a string expansion to an sexp" do
    'a = 1; "hello #{a}, you rock."'.to_sexp.should == 
      [:block, 
        [:lasgn, :a, [:lit, 1]], 
        [:dstr, "hello ", [:evstr, [:lvar, :a, 0]], 
        [:str, ", you rock."]]]
  end

  it "converts a pathological string expansion to an sexp" do
    '@thing = 5; "hello #@thing, you are crazy."'.to_sexp.should == 
      [:block, 
        [:iasgn, :@thing, [:lit, 5]], 
        [:dstr, "hello ", [:evstr, [:ivar, :@thing]], 
        [:str, ", you are crazy."]]]
  end

  it "converts a method definition without arguments to an sexp" do
    "def name; 1; end".to_sexp.should == [:defn, :name, [:scope, [:block, [:args], [:lit, 1]], []]]
  end

  it "converts a method definition with arguments to an sexp" do
    "def name(a, b); 1; end".to_sexp.should == 
      [:defn, :name, 
        [:scope, [:block, [:args, [:a, :b], [], nil, nil], [:lit, 1]], [:a, :b]]]
  end

  it "converts a class definition to an sexp" do
    "class Blah < A::B; end".to_sexp.should == 
      [:class, 
        [:colon2, :Blah], [:colon2, [:const, :A], :B], [:scope, []]]
  end

  it "converts a heredoc to an sexp" do
      "a = <<-BLAH
      hello
      BLAH
      ".to_sexp.should == [:lasgn, :a, [:str, "      hello\n"]]
  end
end

def rewrite_expected array
  case array.first
  when :alias, :undef then
    array[1..-1] = array[1..-1].map { |lit| lit.last }
  when :class, :module then
    name = array[1]
    name = [:colon2, name] unless Array === name and name[0] == :colon2
    array[1] = name
    array[-1] << []      # no clue
  when :defn then
    array[-1] << []      # no clue
  when :lit then
    case array[1]
    when Regexp then
      regexp = array[1]
      array[0..-1] = [:regex, regexp.source, regexp.options - 16] # FIX: broken on kcode
    when Range then
      range = array[1]
      type = range.exclude_end? ? :dot3 : :dot2
      array[0..-1] = [type, [:lit, range.begin], [:lit, range.end]] # FIX
    end
  when :yield then
    array << true
  when :resbody then
    until array.size >= 3 do
      array << nil << nil
    end
  when :lvar then
    array << 0           # depth - seems unused
  end

  array = array.map { |item|
    if Array === item then
      rewrite_expected(item)
    else
      item
    end
  }

  array
end

describe "Producing sexps from source code" do
  SEXP_EXPECTATIONS.each do |node, hash|
    it "succeeds for a node of type :#{node}" do
      expected = rewrite_expected(hash['ParseTree'])
      actual = hash['Ruby'].to_sexp
      actual.should == expected
    end
  end
end
