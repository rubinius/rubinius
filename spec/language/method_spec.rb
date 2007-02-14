require File.dirname(__FILE__) + '/../spec_helper'

# Language-level method behaviour


context 'Defining methods with *' do
  specify 'If * by itself is the only param, method takes any number of args that are ignored' do
    example do
      def foo(*); end;

      foo
      foo 1, 2
      foo 1, 2, 3, 4, :a, :b, 'c', 'd'
    end.should == nil
  end

  specify 'With a parameter name, * stores all extra arguments as an Array in it' do
    example do
      def foo(*a); a; end;

      [foo, foo(1, 2), foo([:a])]
    end.should == [[], [1, 2], [[:a]]]
  end

  specify 'A * param may be preceded by any number of other parameter names' do
    example do
      def foo(a, b, c, d, e, *f); [a, b, c, d, e, f]; end
      foo 1, 2, 3, 4, 5, 6, 7, 8
    end.should == [1, 2, 3, 4, 5, [6, 7, 8]]
  end

  specify 'Only one *param may appear in a parameter list' do
    example do
      begin
        eval 'def foo(a, *b, *c); end'
      rescue SyntaxError
        :success
      end
    end.should == :success
  end

  specify 'The required arguments must be supplied even with a * param' do
    example do
      def foo(a, b, *c); end
      begin
        foo 1
      rescue ArgumentError
        :success
      end
    end.should == :success
  end
end
