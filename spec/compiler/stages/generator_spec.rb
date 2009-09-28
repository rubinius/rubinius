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
