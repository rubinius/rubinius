require File.dirname(__FILE__) + '/../spec_helper'

describe "A Symbol literal" do
  it "is a ':' followed by any number of valid characters" do
    a = :foo
    a.should be_kind_of(Symbol)
    a.inspect.should == ':foo'
  end

  it "is a ':' followed by any valid variable, method, or constant name" do
    # Add more of these?
    [ :Foo,
      :foo,
      :@foo,
      :@@foo,
      :$foo,
      :_,
      :~,
      :- ,
      :FOO,
      :_Foo,
      :&,
      :_9
    ].each { |s| s.should be_kind_of(Symbol) }
  end

  it "is a ':' followed by a single- or double-quoted string that may contain otherwise invalid characters" do
    [ [:'foo bar',      ':"foo bar"'],
      [:'++',           ':"++"'],
      [:'9',            ':"9"'],
      [:"foo #{1 + 1}", ':"foo 2"'],
    ].each { |sym, str| 
      sym.should be_kind_of(Symbol)
      sym.inspect.should == str
    }
  end

  it "may contain '::' in the string" do
    :'Some::Class'.should be_kind_of(Symbol)
  end

  it "is converted to a literal, unquoted representation if the symbol contains only valid characters" do
    a, b, c = :'foo', :'+', :'Foo__9'
    a.class.should == Symbol
    a.inspect.should == ':foo'
    b.class.should == Symbol
    b.inspect.should == ':+'
    c.class.should == Symbol
    c.inspect.should == ':Foo__9'
  end

  it "must not be an empty string" do
    lambda { eval ":''" }.should raise_error(SyntaxError)
  end

  it "can be created by the %s-delimited expression" do
    a, b = :'foo bar', %s{foo bar}
    b.class.should == Symbol
    b.inspect.should == ':"foo bar"'
    b.should == a
  end

  it "is the same object when created from identical strings" do
    var = "@@var"
    [ [:symbol, :symbol],
      [:'a string', :'a string'],
      [:"#{var}", :"#{var}"]
    ].each { |a, b|
      a.should equal(b)
    }
  end
end
