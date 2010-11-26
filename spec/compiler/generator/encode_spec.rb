require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Generator#encode" do
  before :each do
    @g = Rubinius::Generator.new
    @g.set_line 1
  end

  it "raises a CompileError if the method exits with an empty stack" do
    @g.ret
    @g.close

    lambda { @g.encode }.should raise_error(Rubinius::CompileError)
  end

  it "raises a CompileError if pop is called on an empty stack" do
    @g.pop
    @g.ret
    @g.close

    lambda { @g.encode }.should raise_error(Rubinius::CompileError)
  end

  it "raises a CompileError if a BasicBlock's minimum stack size is negative" do
    label = @g.new_label
    @g.push :nil
    @g.gif label
    label.set!
    @g.pop
    @g.push :true
    @g.ret
    @g.close

    lambda { @g.encode }.should raise_error(Rubinius::CompileError)
  end
end
