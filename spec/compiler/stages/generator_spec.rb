require File.dirname(__FILE__) + "/../../spec_helper"

describe Compiler::Generator do
  it "emits a pop normally" do
    g = Compiler::Generator.new
    g.push :nil
    g.send :to_s, 0
    g.pop

    g.stream.should == [:push_nil, [:send_method, 0], :pop]
  end

  it "eliminates push self followed by pop" do
    g = Compiler::Generator.new
    g.push :self
    g.pop

    g.stream.should be_empty
    g.ip.should == 0
  end

  it "eliminates push nil followed by pop" do
    g = Compiler::Generator.new
    g.push :nil
    g.pop

    g.stream.should be_empty
    g.ip.should == 0
  end

  it "eliminates push true followed by pop" do
    g = Compiler::Generator.new
    g.push :true
    g.pop

    g.stream.should be_empty
    g.ip.should == 0
  end

  it "eliminates push false followed by pop" do
    g = Compiler::Generator.new
    g.push :false
    g.pop

    g.stream.should be_empty
    g.ip.should == 0
  end

  it "eliminates push {-1,0,1,2} followed by pop" do
    g = Compiler::Generator.new
    g.push(-1)
    g.pop

    g.stream.should be_empty

    g.push 0
    g.pop

    g.stream.should be_empty

    g.push 1
    g.pop

    g.stream.should be_empty

    g.push 2
    g.pop

    g.stream.should be_empty
  end

  it "eliminates push small number followed by pop" do
    g = Compiler::Generator.new
    g.push 3
    g.pop

    g.stream.should be_empty
    g.ip.should == 0
  end

  it "eliminates cast_array following a make_array" do
    g = Compiler::Generator.new
    g.make_array 0
    g.cast_array

    g.stream.should == [[:make_array, 0]]
  end

  it "eliminates cast_array following a cast_array" do
    g = Compiler::Generator.new
    g.cast_array
    g.cast_array

    g.stream.should == [:cast_array]
  end

  it "eliminates cast_array following a cast_array again" do
    g = Compiler::Generator.new
    g.cast_array
    g.cast_array
    g.cast_array

    g.stream.should == [:cast_array]
  end

  it "handles multiple removals in a row" do
    g = Compiler::Generator.new
    g.push_nil
    g.pop
    g.push_nil
    g.pop
    g.push_nil
    g.pop
    g.push_nil
    g.pop
    g.push_nil
    g.pop
    g.push_nil
    g.pop
    g.push_nil
    g.pop

    g.stream.should be_empty
  end

end

describe Compiler::Generator, "stack size calculator" do
  it "calculates stack of '1 + 1'" do
    g = Compiler::Generator.new
    g.push 1
    g.push 1
    g.meta_send_op_plus
    g.pop

    max, cur = g.instruction_stack_depth
    max.should == 2
    cur.should == 1
  end

  it "calculates stack depth through gotos" do
#:push_nil                     [1, 0, 0]
#[:set_local, 0]               [0, 1, 1]
#:pop                          [-1, 1, 1]
#[:push_local, 0]              [1, 0, 1]
#[:send_method, 0]             [0, 1, 1]
#:meta_push_0                  [1, 1, 1]
#[:send_stack, 1, 1]           [-1, 2, 2]
#[:set_local, 1]               [0, 1, 2]
#:pop                          [-1, 1, 2]
#[:push_local, 1]              [1, 0, 2]
#:dup_top                      [1, 1, 2]
#[:push_int, 116]              [1, 2, 2]
#:swap_stack                   [0, 3, 3]
#[:send_stack, 2, 1]           [-1, 3, 3]
#[:goto_if_false, 31]          [-1, 2, 3]
#:pop                          [-1, 1, 3]
#:push_true                    [1, 0, 3]
#:ret                          [-1, 1, 3]
#[:goto, 795]                  [0, 0, 3]
#:dup_top                      [1, 0, 3]
#[:push_int, 102]              [1, 1, 3]
#:swap_stack                   [0, 2, 3]
#[:send_stack, 3, 1]           [-1, 2, 3]
 
    g = Compiler::Generator.new
    nxt = g.new_label
    don = g.new_label

    g.push_nil
    g.set_local 0
    g.pop
    g.push_local 0
    g.send :blah, 0
    g.push 0
    g.send :go, 1
    g.set_local 1
    g.pop
    g.push_local 1
    g.dup
    g.push 116
    g.swap
    g.send :sweet, 1
    g.gif nxt
    g.pop
    g.push :true
    g.ret
    g.goto don

    nxt.set!
    g.dup
    don.set!

    max, cur = g.instruction_stack_depth
    max.should == 3
    cur.should == 2
  end
end
