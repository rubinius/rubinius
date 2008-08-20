require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#lineno" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.lineno }.should raise_error(IOError)
  end

  it "returns the current line number" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      f.lineno.should == 0
      while (f.gets)
        f.lineno.should > 0
      end
      f.rewind
      f.lineno.should == 0
    end
  end
end

describe "IO#lineno=" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.lineno = 5 }.should raise_error(IOError)
  end

  it "invokes to_int on non-numeric arguments" do
    (obj = mock('123')).should_receive(:to_int).and_return(123)
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      f.lineno = obj
      f.lineno.should == 123

      f.lineno = 1.5
      f.lineno.should == 1

      f.lineno = 92233.72036854775808
      f.lineno.should == 92233
    end
  end

  it "raises TypeError on nil argument" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      lambda { f.lineno = nil }.should raise_error(TypeError)
    end
  end

  it "sets the current line number to the given value" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      count = 500
      f.lineno = count - 1
      while (f.gets)
        f.lineno.should == count
        count += 1
      end
      f.rewind
      f.lineno.should == 0
    end
  end

  it "does not change $." do
    orig_value = $.
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      numbers = [-2**30, -2**16, -2**8, -100, -10, -1, 0, 1, 10, 2**8, 2**16, 2**30]
      numbers.each { |num|
        f.lineno = num
        f.lineno.should == num
        $..should == orig_value
      }
    end
  end

  it "does not change $. until next read" do
    $. = 0
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      $..should == 0
      count = 500
      f.lineno = count - 1
      $..should == 0
      while (f.gets)
        $..should == count
        count += 1
      end
    end
  end
end
