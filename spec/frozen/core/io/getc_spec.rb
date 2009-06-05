# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#getc" do
  before :each do
    @original = $KCODE
    $KCODE = "UTF-8"
    @file_name = File.dirname(__FILE__) + '/fixtures/readlines.txt'
    @file = File.open(@file_name, 'r')
  end

  after :each do
    @file.close unless @file.closed?
    $KCODE = @original
  end

  ruby_version_is ''...'1.9' do
    it "returns the next byte from the stream" do
      @file.readline.should == "Voici la ligne une.\n"
      letters = @file.getc, @file.getc, @file.getc, @file.getc, @file.getc
      letters.should == [81, 117, 105, 32, 195]
    end
  end

  ruby_version_is '1.9' do
    it "returns the next character from the stream" do
      @file.readline.should == "Voici la ligne une.\n"
      letters = @file.getc, @file.getc, @file.getc, @file.getc, @file.getc
      letters.should == ["Q", "u", "i", " ", "è"]
    end
  end

  it "returns nil when invoked at the end of the stream" do
    # read entire content
    @file.read
    @file.getc.should == nil
  end

  it "returns nil on empty stream" do
    File.open(tmp('empty.txt'), "w+") { |empty|
      empty.getc.should == nil
    }
    File.unlink(tmp("empty.txt"))
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.getc }.should raise_error(IOError)
  end
end
