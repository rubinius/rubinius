require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#readline" do

  it "returns the next line on the stream" do
    testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'
    f = File.open(testfile, 'r') do |f|
      f.readline.should == "Voici la ligne une.\n"
      f.readline.should == "Qui è la linea due.\n"
    end
  end

  it "goes back to first position after a rewind" do
    testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'
    f = File.open(testfile, 'r') do |f|
      f.readline.should == "Voici la ligne une.\n"
      f.rewind
      f.readline.should == "Voici la ligne une.\n"
    end
  end

  it "is modified by the cursor position" do
    testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'
    f = File.open(testfile, 'r') do |f|
      f.seek(1)
      f.readline.should == "oici la ligne une.\n"
    end
  end

  it "raises EOFError on end of stream" do
    testfile = File.dirname(__FILE__) + '/fixtures/gets.txt'
    File.open(testfile, 'r') do |f|
      lambda { loop { f.readline } }.should raise_error(EOFError)
    end

  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.readline }.should raise_error(IOError)
  end
end
