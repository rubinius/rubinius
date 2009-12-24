require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#reopen" do
  before :each do
    # for reading
    @name1 = IOSpecs.gets_fixtures
    @name2 = File.dirname(__FILE__) + '/fixtures/numbered_lines.txt'
    @file1 = File.new(@name1)
    @file2 = File.new(@name2)

    # for writing
    @name1_w = tmp("IO_reopen_file1") + $$.to_s
    @name2_w = tmp("IO_reopen_file2") + $$.to_s
    @file1_w = File.new(@name1_w, "w+")
    @file2_w = File.new(@name2_w, "w+")
  end

  after :each do
    @file1.close unless @file1.closed?
    @file2.close unless @file2.closed?
    @file1_w.close unless @file1_w.closed?
    @file2_w.close unless @file2_w.closed?
    rm_r @name1_w, @name2_w
  end

  it "raises IOError on closed stream" do
    File.open(File.dirname(__FILE__) + '/fixtures/gets.txt', 'r') { |f|
      lambda { f.reopen(IOSpecs.closed_file) }.should raise_error(IOError)
    }
  end

  it "raises IOError when called on closed stream" do
    @file1.close
    lambda { @file1.reopen(@file2) }.should raise_error(IOError)
  end

  it "should not raise IOError when called on closed stream with path" do
    @file1.close
    lambda do
      @file1.reopen(@name2, "r")
    end.should_not raise_error(IOError)

    @file1.closed?.should be_false
    @file1.gets.should == "Line 1: One\n"
  end

  it "reassociates self to another file/descriptor but returns self" do
    @file1.reopen(@file2).should == @file1
    @file2.reopen(@file1).should == @file2
    @file1.reopen(@name2).should == @file1
    @file2.reopen(@name2).should == @file2
  end
  
  it "reassociates self with a new stream opened on path, when self in initial state" do
    @file1.reopen(@name2)
    @file1.gets.should == "Line 1: One\n"
  end

  it "reassociates self with a new stream opened on path, after some reads" do
    # reade some first
    4.times {@file1.gets; @file2.gets}

    @file1.reopen(@name2)
    @file1.gets.should == "Line 1: One\n"
  end

  it "reassociates self with a new stream opened on path, after some writes" do
    @file1_w.puts("line1-F1")
    @file2_w.puts("line1-F2")
    @file2_w.reopen(@name1_w)
    @file1_w.puts("line2-F1")
    @file2_w.puts("line2-F2")
    @file1_w.close
    @file2_w.close
    File.readlines(@name1_w).should == ["line2-F2\n", "line2-F1\n"]
    File.readlines(@name2_w).should == ["line1-F2\n"]
  end

  # JRUBY-2071: File#reopen blows with IllegalArgumentException in some cases
  it "reassociates self with the I/O stream specified as an argument, after some reads" do
    length = 12 # length of first lines in numbered_lines.txt

    # read some first
    @file1.gets
    @file2.gets

    pos = @file2.pos
    @file1.reopen(@file2)
    @file1.pos.should == pos

    # MRI behavior: after reopen the buffers are not corrected,
    # so we need the following line, or next gets wourd return nil.
    @file1.pos = pos

    @file1.gets.should == "Line 2: Two\n"
  end

  platform_is_not :darwin, :freebsd do
    it "reassociates self with the I/O stream specified as an argument, after some sysreads" do
      length = 12 # length of first lines in numbered_lines.txt

      # reade some first
      @file1.sysread(length)
      @file2.sysread(length)

      @file1.reopen(@file2)
      @file1.sysread(length).should == "Line 2: Two\n"
    end
  end

  it "reassociates self with the I/O stream specified as an argument, after some writes" do
    @file1_w.puts("line1-F1")
    @file2_w.puts("line1-F2")
    @file2_w.reopen(@file1_w)
    @file1_w.puts("line2-F1")
    @file2_w.puts("line2-F2")
    @file1_w.close
    @file2_w.close
    File.readlines(@name1_w).should == ["line1-F1\n", "line2-F1\n", "line2-F2\n"]
    File.readlines(@name2_w).should == ["line1-F2\n"]
  end

  it "reassociates self with a new stream after some reads" do
    @file1.reopen(@file2)
    @file1.gets
    @file1.gets
    @file1.reopen(@file2).gets.should == "Line 1: One\n"
  end

  ruby_version_is "1.9" do
    it "calls #to_path on non-String arguments" do
      p = mock('path')
      p.should_receive(:to_path).and_return(@file2.to_path)
      @file1.reopen(p)
    end
  end
end
