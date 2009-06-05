# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#gets" do
  after :each do
    File.delete IOSpecs.gets_output if File.exists?(IOSpecs.gets_output)
  end

  it "returns the next line of string that were separated by $/" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      IOSpecs.lines.each {|line| line.should == f.gets}
    end
  end

  it "returns tainted strings" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets(nil))
        line.tainted?.should == true
      end
    end

    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets(""))
        line.tainted?.should == true
      end
    end

    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets)
        line.tainted?.should == true
      end
    end

    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets("la"))
        line.tainted?.should == true
      end
    end
  end

  it "updates lineno with each invocation" do
    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets(nil))
        f.lineno.should == count
        count += 1
      end
    end

    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets(""))
        f.lineno.should == count
        count += 1
      end
    end

    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets)
        f.lineno.should == count
        count += 1
      end
    end

    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets("la"))
        f.lineno.should == count
        count += 1
      end
    end
  end

  it "updates $. with each invocation" do
    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets(nil))
        $..should == count
        count += 1
      end
    end

    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets(""))
        $..should == count
        count += 1
      end
    end

    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets)
        $..should == count
        count += 1
      end
    end

    count = 1
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      while (line = f.gets("la"))
        $..should == count
        count += 1
      end
    end
  end

  it "assigns the returned line to $_" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      IOSpecs.lines.each do |line|
        f.gets
        $_.should == line
      end
    end
  end

  it "returns nil if called at the end of the stream" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      IOSpecs.lines.length.times { f.gets }
      f.gets.should == nil
    end
  end

  it "returns the entire content if the separator is nil" do
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      f.gets(nil).should == IOSpecs.lines.join('')
    end
  end

  # Two successive newlines in the input separate paragraphs.
  # When there are more than two successive newlines, only two are kept.
  it "returns the next paragraph if the separator's length is 0" do
    a = "Voici la ligne une.\nQui \303\250 la linea due.\n\n"
    b = "Aqu\303\255 est\303\241 la l\303\255nea tres.\nIst hier Linie vier.\n\n"
    c = "Est\303\241 aqui a linha cinco.\nHere is line six.\n"

    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      f.gets("").should == a
      f.gets("").should == b
      f.gets("").should == c
    end
  end

  # This could probably be added to the previous test using pos, but right now 
  # this doesn't pass and the previous test does.
  it "reads until the beginning of the next paragraph when the separator's length is 0" do
    # Leverage the fact that there are three newlines between the first 
    # and second paragraph
    File.open(IOSpecs.gets_fixtures, 'r') do |f|
      f.gets('')

      # This should return 'A', the first character of the next paragraph, not $/
      f.read(1).should == 'A'
    end
  end

  it "raises an IOError if the stream is not opened for reading" do
    path = tmp("gets_spec")
    lambda { File.open(path, 'a') {|f| f.gets} }.should raise_error(IOError)
    lambda { File.open(path, 'w') {|f| f.gets} }.should raise_error(IOError)
    File.unlink(path) if File.exist?(path)
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.gets }.should raise_error(IOError)
  end

  it "fails on cloned opened streams" do
    f = File.open(tmp("gets_specs"), "w")
    f.puts("heh")
    g = IO.new(f.fileno)
    f.fileno.should == g.fileno
    lambda { g.gets }.should raise_error(IOError)
    g.close
    File.unlink(tmp("gets_specs"))
  end

  it "accepts a separator" do
    f = File.open(tmp("gets_specs"), "w")
    f.print("A\n\n\nB\n")
    f.close
    f = File.new(tmp("gets_specs"), "r")
    f.gets("\n\n")
    b = f.gets("\n\n")
    f.gets("\n\n")
    f.close
    b.should == "\nB\n"
    File.unlink(tmp("gets_specs"))
  end
end
