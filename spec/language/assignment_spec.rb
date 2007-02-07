require File.dirname(__FILE__) + '/../spec_helper'


context 'Single assignment' do
  specify 'Assignment does not modify the lhs, it reassigns its reference' do
    example do
      a = 'Foobar'
      b = a
      b = 'Bazquux'
      [a == 'Foobar',
       b == 'Bazquux']
    end.should == [true, true]
  end

  specify 'Assignment does not copy the object being assigned, just creates a new reference to it' do
    example do
      a = []
      b = a
      b << 1
      a
    end.should == [1]
  end

  specify 'If rhs has multiple arguments, lhs becomes an Array of them' do
    example do
      a = 1, 2, 3
      a == [1, 2, 3]
    end.should == true
  end
end

context 'Multiple assignment without grouping or splatting' do
  specify 'An equal number of arguments on lhs and rhs assigns positionally' do
    example do
      a, b, c, d = 1, 2, 3, 4
      [a == 1, b == 2, c == 3, d == 4]
    end.should == [true, true, true, true]
  end 

  specify 'If rhs has too few arguments, the missing ones on lhs are assigned nil' do
    example do
      a, b, c = 1, 2
      [a == 1, b == 2, c == nil]
    end.should == [true, true, true]
  end

  specify 'If rhs has too many arguments, the extra ones are silently not assigned anywhere' do
    example do
      a, b = 1, 2, 3
      [a == 1, b == 2]
    end.should == [true, true]
  end

  specify 'The assignments are done in parallel so that lhs and rhs are independent of eachother without copying' do
    example do
      o_of_a, o_of_b = Object.new, Object.new
      a, b = o_of_a, o_of_b

      a, b = b, a

      [a.equal?(o_of_b), b.equal?(o_of_a)]
    end.should == [true, true]
  end
end


context 'Multiple assignments with grouping' do
  specify 'A group on the lhs is considered one position and treats its corresponding rhs position like an Array' do
    example do
      a, (b, c), d = 1, 2, 3, 4
      e, (f, g), h = 1, [2, 3, 4], 5
      i, (j, k), l = 1, 2, 3

      [a == 1, b == 2, c == nil, d == 3,
       e == 1, f == 2, g == 3, h == 5,
       i == 1, j == 2, k == nil, l == 3]
    end.all? {|x| x == true}.should == true
  end

  specify 'rhs cannot use parameter grouping, it is a syntax error' do
    example do
      begin 
        eval '(a, b) = (1, 2)'
      rescue SyntaxError
        :success
      end
    end.should == :success
  end
end


context 'Multiple assignments with splats' do
  specify '* on the lhs has to be applied to the last parameter' do
    example do
      begin
        eval 'a, *b, c = 1, 2, 3'
      rescue SyntaxError
        :success
      end
    end.should == :success
  end

  specify '* on the lhs collects all parameters from its position onwards as an Array or an empty Array' do
    example do
      a, *b = 1, 2
      c, *d = 1
      e, *f = 1, 2, 3
      g, *h = 1, [2, 3]

      [a == 1, b == [2],
       c == 1, d == [],
       e == 1, f == [2, 3],
       g == 1, h == [[2, 3]]]
    end.all? {|x| x == true}.should == true
  end
end
