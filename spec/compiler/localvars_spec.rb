require(File.dirname(__FILE__) + '/compiler_helper')

c = CompilerHarness.new()

context "Local variable access" do
  specify "lvar in method body uses specific syntax" do
    code = <<-CODE
    a = 3
    p a
    CODE
    
    asm = <<-CODE
#line 1
push 3
set a:2
pop
#line 2
push a:2
push self
send p 1
ret
    CODE
    
    c.encode(code).should == asm
  end
  
  specify "lvar defined in method, accessed in block" do
    code = <<-CODE
    a = 3
    [].each do |b|
      c = 8
      p(a, b)
    end
    CODE
    
    asm = <<-CODE
#line 1
push 3
set a:2
pop
#line 2
push &lbl1
push &lbl2
create_block 2
goto lbl3
unshift_tuple
set b:0:0
pop
lbl4:
#line 3
push 8
set c:0:1
pop
#line 4
push b:0:0
push a:2
push self
send p 2
lbl2: soft_return
lbl3:
make_array 0
&send each 0
lbl1:
ret
    CODE
    
    c.encode(code).should == asm
  end  
end