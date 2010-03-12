# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/gets_ascii', __FILE__)

describe "IO#gets" do
  it_behaves_like :io_gets_ascii, :gets
end

describe "IO#gets" do
  before :each do
    @io = IOSpecs.io_fixture "lines.txt"
    @count = 0
  end

  after :each do
    @io.close unless @io.closed?
  end

  it "assigns the returned line to $_" do
    IOSpecs.lines.each do |line|
      @io.gets
      $_.should == line
    end
  end

  it "returns nil if called at the end of the stream" do
    IOSpecs.lines.length.times { @io.gets }
    @io.gets.should == nil
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.gets }.should raise_error(IOError)
  end

  describe "with no separator" do
    it "returns the next line of string that is separated by $/" do
      IOSpecs.lines.each { |line| line.should == @io.gets }
    end

    it "returns tainted strings" do
      while line = @io.gets
        line.tainted?.should == true
      end
    end

    it "updates lineno with each invocation" do
      while line = @io.gets
        @io.lineno.should == @count += 1
      end
    end

    it "updates $. with each invocation" do
      while line = @io.gets
        $..should == @count += 1
      end
    end
  end

  describe "with nil separator" do
    it "returns the entire contents" do
      @io.gets(nil).should == IOSpecs.lines.join("")
    end

    it "returns tainted strings" do
      while line = @io.gets(nil)
        line.tainted?.should == true
      end
    end

    it "updates lineno with each invocation" do
      while line = @io.gets(nil)
        @io.lineno.should == @count += 1
      end
    end

    it "updates $. with each invocation" do
      while line = @io.gets(nil)
        $..should == @count += 1
      end
    end
  end

  describe "with an empty String separator" do
    # Two successive newlines in the input separate paragraphs.
    # When there are more than two successive newlines, only two are kept.
    it "returns the next paragraph" do
      @io.gets("").should == IOSpecs.lines[0,3].join("")
      @io.gets("").should == IOSpecs.lines[4,3].join("")
      @io.gets("").should == IOSpecs.lines[7,2].join("")
    end

    it "reads until the beginning of the next paragraph" do
      # There are three newlines between the first and second paragraph
      @io.gets("")
      @io.gets.should == IOSpecs.lines[4]
    end

    it "returns tainted strings" do
      while line = @io.gets("")
        line.tainted?.should == true
      end
    end

    it "updates lineno with each invocation" do
      while line = @io.gets("")
        @io.lineno.should == @count += 1
      end
    end

    it "updates $. with each invocation" do
      while line = @io.gets("")
        $..should == @count += 1
      end
    end
  end

  describe "with an arbitrary String separator" do
    it "reads up to and including the separator" do
      @io.gets("la linea").should == "Voici la ligne une.\nQui \303\250 la linea"
    end

    it "returns tainted strings" do
      while line = @io.gets("la")
        line.tainted?.should == true
      end
    end

    it "updates lineno with each invocation" do
      while (line = @io.gets("la"))
        @io.lineno.should == @count += 1
      end
    end

    it "updates $. with each invocation" do
      while line = @io.gets("la")
        $..should == @count += 1
      end
    end
  end
end

describe "IO#gets" do
  before :each do
    @name = tmp("io_gets")
  end

  after :each do
    rm_r @name
  end

  it "raises an IOError if the stream is opened for append only" do
    lambda { File.open(@name, fmode("a:utf-8")) { |f| f.gets } }.should raise_error(IOError)
  end

  it "raises an IOError if the stream is opened for writing only" do
    lambda { File.open(@name, fmode("w:utf-8")) { |f| f.gets } }.should raise_error(IOError)
  end
end

ruby_version_is "1.9" do
  describe "IO#gets" do
    before :each do
      @name = tmp("gets_specs")
    end

    after :each do
      @io.close
      rm_r @name
    end

    it "accepts an integer as first parameter to limit the output's size" do
      touch(@name) { |f| f.print("waduswadus") }

      @io = new_io @name, fmode("r:utf-8")
      @io.gets(5).should == "wadus"
    end

    it "accepts an integer as second parameter to limit the output's size" do
      touch(@name) { |f| f.print("wa\n\ndus\n\nwadus") }

      @io = new_io @name, fmode("r:utf-8")
      @io.gets('\n\n', 5).should == "wa\n\nd"
    end

    it "accepts an integer as limit parameter which is smaller than IO size" do
      touch(@name) { |f| f.print("ABCD\n") }

      @io = new_io @name, fmode("r:utf-8")
      @io.gets("", 2).should == "AB"
    end

    it "accepts an integer as limit parameter which is same as IO size" do
      touch(@name) { |f| f.print("ABC\n") }

      @io = new_io @name, fmode("r:utf-8")
      @io.gets("", 4).should == "ABC\n"
    end

    it "accepts an integer as limit parameter which is greater than IO size" do
      touch(@name) { |f| f.print("A\n") }

      @io = new_io @name, fmode("r:utf-8")
      @io.gets("", 10).should == "A\n"
    end
  end
end
