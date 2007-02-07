require File.dirname(__FILE__) + '/../../spec_helper'

context 'Using the literal form to create Symbols' do
  specify 'Colon followed by a valid series of characters creates a Symbol' do
    example do
      a = :foo
      [a.class == Symbol, a.inspect == ':foo', a == :foo]
    end.all? {|x| x == true}.should == true
  end

  specify 'Anything that would be a valid variable, method or constant name can be used in a literal Symbol' do
    example do
      # Add more of these?
      [:Foo,
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
       :_9]
    end.all? {|x| Symbol === x}.should == true
  end

  specify 'Colon followed by a single- or double-quoted String creates a Symbol which may contain nonvalid characters' do
    example do
      [:'foo bar'.class, :'foo bar'.inspect,
       :'++'.class, :'++'.inspect,
       :'9'.class, :'9'.inspect,
       :"foo #{1 + 1}".class, :"foo #{1 + 1}".inspect] 
    end.should == [Symbol, ':"foo bar"',
                   Symbol, ':"++"',
                   Symbol, ':"9"',
                   Symbol, ':"foo 2"']
  end

  specify 'If the String consists of valid characters only, the representation is converted to a plain literal' do
    example do
      a, b, c = :'foo', :'+', :'Foo__9'
      [a.class, a.inspect,
       b.class, b.inspect,
       c.class, c.inspect]
    end.should == [Symbol, ':foo', Symbol, ':+', Symbol, ':Foo__9']
  end

  specify 'The string may not be empty' do
    example do
      begin 
        eval ":''"
      rescue Exception
        :failure
      end
    end.should == :failure
  end

  specify '%s general-delimited expression creates Symbols like a single-quoted String' do
    example do
      a, b = :'foo bar', %s{foo bar}
      [b.class, b.inspect, b == a]
    end.should == [Symbol, ':"foo bar"', true]
  end

  specify 'Each time a Symbol is constructed, the same object is returned' do
    example do
      var = 'foo'
      a, b, c = :foo, :foo, :foo 
      d, e, f = :'foo', :'foo', :'foo'
      g, h, i = :"#{var}", :"#{var}", :"#{var}"

      a.equal? b and b.equal? c and
      c.equal? d and d.equal? e and
      e.equal? f and f.equal? g and
      g.equal? h and h.equal? i
    end.should == true
  end
end
