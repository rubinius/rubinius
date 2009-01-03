require File.dirname(__FILE__) + '/../../spec_helper'
require "stringio"
require File.dirname(__FILE__) + '/shared/read'

describe "StringIO#read when passed length, buffer" do
  it_behaves_like :stringio_read, :read
end

describe "StringIO#read when passed [length]" do
  it_behaves_like :stringio_read_length, :read
end

describe "StringIO#read when passed no arguments" do
  it_behaves_like :stringio_read_no_arguments, :read
end

describe "StringIO#read when self is not readable" do
  it_behaves_like :stringio_read_not_readable, :read
end

describe "StringIO#read when passed [length]" do
  before(:each) do
    @io = StringIO.new("example")
  end
  
  it "returns nil when self's position is at the end" do
    @io.pos = 7
    @io.read(10).should be_nil
  end

  it "returns an empty String when length is 0" do
    @io.read(0).should == ""
  end
end
