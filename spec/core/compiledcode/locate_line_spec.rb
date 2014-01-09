require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CompiledCode#locate_line" do
  before :each do
    @line_test = CompiledCodeSpecs.executable(:line_test)
    @block_line_test = CompiledCodeSpecs.executable(:block_line_test)
  end

  it "finds the ip for a line" do
    _, ip = @line_test.locate_line(7)
    ip.should == 6
  end

  it "returns the CompiledCode the ip is in" do
    cm, _ = @line_test.locate_line(6)
    cm.should == @line_test
  end

  it "returns nil if the line can't be found" do
    @line_test.locate_line(5000).should be_nil
  end

  it "returns nil if the line is the definition line" do
    @line_test.locate_line(5).should be_nil
  end

  it "searches blocks for the line" do
    cm, ip = @block_line_test.locate_line(13)
    blk = @block_line_test.literals.to_a.grep(Rubinius::CompiledCode).first

    cm.should == blk
    ip.should == 0
  end
end
