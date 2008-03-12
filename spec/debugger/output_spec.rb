require File.dirname(__FILE__) + "/spec_helper.rb"

describe "Output::Columns#wrap" do
  before :each do
    @cols = Debugger::Output::Columns.new 1
  end

  it "returns a single element array containing the input string if arg length is less than the wrap length" do
    @cols.wrap(nil, 40).should == [nil]
    @cols.wrap("", 40).should == [""]
    @cols.wrap("here is a short string", 40).should == ["here is a short string"]
  end

  it "returns nil if wrap == 0" do
    @cols.wrap(nil, 0).should == [nil]
    @cols.wrap("here is a string", 0).should == [nil]
  end

  it "raises an ArgumentError if wrap < 0" do
    lambda{ @cols.wrap(nil, -1) }.should raise_error(ArgumentError)
    lambda{ @cols.wrap("here is a string", -1) }.should raise_error(ArgumentError)
  end

  it "returns an array of lines when an input string is provided" do
    @cols.wrap("", 1).should == [""]
    @cols.wrap("a", 1).should == ["a"]
    @cols.wrap("ab", 1).should == ["a","b"]
    @cols.wrap("here is a short string", 20).should == ["here is a short", "string"]
  end

  it "breaks long lines on whitespace, comma, or dash first, and then on non-word characters" do
    @cols.wrap("here is a test-45 string", 14).should == ["here is a", "test-45 string"]
    @cols.wrap("here is a test-45 string", 15).should == ["here is a test-", "45 string"]
    @cols.wrap("1,2", 2).should == ["1,", "2"]
    @cols.wrap("here is a CompiledMethod#decode test", 20).should == ["here is a", "CompiledMethod", "#decode test"]
  end

  it "forces a break at width if no other break position exists" do
    @cols.wrap("abcdefghijklmnop", 10).should == ["abcdefghij","klmnop"]
  end

  it "trims leading and trailing whitespace from split lines" do
    @cols.wrap("  45  ", 2).should == ["45"]
    @cols.wrap("  45  54    63 ", 2).should == ["45", "54", "63"]
  end

  it "pads lines with spaces if an alignment is specified" do
    @cols.wrap("abc", 5, :left).should == ["abc  "]
    @cols.wrap("abc", 5, :center).should == [" abc "]
    @cols.wrap("abc", 5, :right).should == ["  abc"]
  end
end


describe "Output::Columns#format_str" do
  it "aligns columns according to a left or right justification specification" do
    @cols = Debugger::Output::Columns.new(["%-5s"], nil, false)
    @cols.format_str(["a"]).should == ["a    "]
    @cols = Debugger::Output::Columns.new(["%5s"], nil, false)
    @cols.format_str(["a"]).should == ["    a"]
    @cols = Debugger::Output::Columns.new(["%5s", "%5s"],"|", false)
    @cols.format_str(["a","b"]).should == ["    a|    b"]
  end

  it "wraps a cell value onto multiple lines if the content of a cell is wider than the cell width" do
    @cols = Debugger::Output::Columns.new(["%10s"], nil, false)
    @cols.format_str(["abcdefghij"]).should == ["abcdefghij"]
    @cols.format_str(["abcdefghijklmnop"]).should == ["abcdefghij","    klmnop"]

    @cols = Debugger::Output::Columns.new(["%-5s","%10s"],"|", false)
    @cols.format_str(["a", "abcdefghijklmnop"]).should == ["a    |abcdefghij", "     |    klmnop"]
    @cols = Debugger::Output::Columns.new(["%-5s","%10s"],"|", true)
    @cols.update_widths(["a", "abcdefghijklmnop"])
    @cols.format_str(["a", "abcdefghijklmnop"]).should == ["a    |abcdefghijklmnop"]
  end

  it "handles columns that calculate a 0 width" do
    @cols = Debugger::Output::Columns.new(["%04d:", "%-s ", "%-s"])
    @cols.update_widths([41, :push_self, ""])
    @cols.format_str([41, :push_self, ""]).should == ["0041: push_self  "]
  end
end