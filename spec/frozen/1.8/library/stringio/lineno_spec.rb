require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#lineno" do
  before(:each) do
    @io = StringIOSpecs.build
  end

  it "only counts lines that have been read 'line-wise'" do
    @io.read(@io.size)
    @io.lineno.should == 0
  end

  it "matches the newline count after each_line" do
    @io.each_line {|l| l }
    @io.lineno.should == 4
  end

  it "matches the number of 'lines' read, even if they are not lines" do
    @io.gets('a')
    @io.gets('c')
    @io.gets('h')
    6.times { @io.gets }
    @io.lineno.should == 7
  end

  it "updates the line number after readline, as well as gets" do
    @io.readline
    @io.readline
    @io.readline
    @io.lineno.should == 3
  end
end

describe "StringIO#lineno=" do
  before(:each) do
    @io = StringIOSpecs.build
  end

  it "updates the current line number but not advance the position" do
    @io.lineno = 2
    @io.gets.should == "    each\n"
    @io.lineno.should == 3
  end
end
