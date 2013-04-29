module CompiledCodeSpec
  LineTest = def line_test
    1 + 1
    2 + 2
  end

  BlockLineTest = def block_line_test
    1 + 1
    1.times do
      2 + 2
    end
  end
end

# Line numbers are important! Thats why the above module is in
# this file and not a fixture.

describe "Rubinius::CompiledCode#locate_line" do
  it "finds the ip for a line" do
    cm, ip = CompiledCodeSpec::LineTest.locate_line(4)
    ip.should == 6
  end

  it "returns the CompiledCode the ip is in" do
    cm, ip = CompiledCodeSpec::LineTest.locate_line(3)
    cm.should == CompiledCodeSpec::LineTest
  end

  it "returns nil if the line can't be found" do
    CompiledCodeSpec::LineTest.locate_line(50).should be_nil
  end

  it "returns nil if the line is the definition line" do
    CompiledCodeSpec::LineTest.locate_line(2).should be_nil
  end

  it "searches blocks for the line" do
    cm, ip = CompiledCodeSpec::BlockLineTest.locate_line(10)
    blk = CompiledCodeSpec::BlockLineTest.literals.to_a.grep(Rubinius::CompiledCode).first

    cm.should == blk
    ip.should == 0
  end
end
