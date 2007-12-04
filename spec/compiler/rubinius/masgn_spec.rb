require File.dirname(__FILE__) + '/compiler_helper'

extension :rubinius do
c = CompilerHarness.new

context 'Single assignment' do
  specify 'If rhs has multiple arguments, lhs becomes an Array of them' do
    code = <<-CODE
      a = 1, 2, 3
    CODE

    asm = <<-ASM
#line 1
push 1
push 2
push 3
make_array 3
set a:2
sret
  ASM

    c.encode(code).should == asm
  end
end

context 'Multiple assignment without grouping or splatting' do
  specify 'An equal number of arguments on lhs and rhs assigns positionally' do
    code = <<-CODE 
      a, b, c, d = 1, 2, 3, 4
    CODE

    asm = <<-ASM
#line 1
push 4
push 3
push 2
push 1
set a:2
pop
set b:3
pop
set c:4
pop
set d:5
pop
sret
    ASM
    
    out = c.encode(code)
    out.should == asm
  end 

  specify 'If rhs has too few arguments, the missing ones on lhs are assigned nil' do
    code = <<-CODE
      a, b, c = 1, 2
    CODE

    asm = <<-ASM
#line 1
push nil
push 2
push 1
set a:2
pop
set b:3
pop
set c:4
pop
sret
    ASM
    
    out = c.encode(code)
    out.should == asm
  end

  specify 'If rhs has too many arguments, the extra ones are silently not assigned anywhere' do
    code = <<-CODE
      a, b = 1, 2, 3
    CODE

    asm = <<-ASM
#line 1
push 3
push 2
push 1
set a:2
pop
set b:3
pop
pop
sret
    ASM
    
    out = c.encode(code)
    out.should == asm
  end
end
end

=begin
context 'Multiple assignments with grouping' do
  specify 'A group on the lhs is considered one position and treats its corresponding rhs position like an Array' do
    a, (b, c), d = 1, 2, 3, 4
    [a, b, c, d].should == [1, 2, nil, 3]
    e, (f, g), h = 1, [2, 3, 4], 5
    [e, f, g, h].should == [1, 2, 3, 5]
    i, (j, k), l = 1, 2, 3
    [i, j, k, l].should == [1, 2, nil, 3]
  end

  specify 'rhs cannot use parameter grouping, it is a syntax error' do
    lambda { eval '(a, b) = (1, 2)' }.should raise_error(SyntaxError)
  end
end
=end

context 'Multiple assignments with splats' do
=begin
  specify '* on the lhs has to be applied to the last parameter' do
    lambda { eval 'a, *b, c = 1, 2, 3' }.should raise_error(SyntaxError)
  end
=end

  specify '* on the lhs collects all parameters from its position onwards as an Array or an empty Array' do
    a, *b = 1, 2
    [a, b].should == [1, [2]]
    c, *d = 1
    [c, d].should == [1, []]
    e, *f = 1, 2, 3
    [e, f].should == [1, [2, 3]]
    g, *h = 1, [2, 3]
    [g, h].should == [1, [[2, 3]]]
    i, *j = [1, 2, 3].dup
    [i, j].should == [1, [2, 3]]
  end
end
