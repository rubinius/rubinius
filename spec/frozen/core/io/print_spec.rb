require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe IO, "#print" do
  class IOSpecPrint
    attr_accessor :message
    def to_s; @message; end
  end

  before :each do
    @old_separator = $\
    $\ = '->'
  end

  after :each do
    $\ = @old_separator
  end

  it "writes $_.to_s followed by $\\ (if any) to the stream if no arguments given" do
    o = IOSpecPrint.new
    o.message = 'I know what you did last line!'
    $_ = o
    l = lambda { $stdout.print }.should output_to_fd("#{o.message}#{$\}", STDOUT)

    string = File.open(__FILE__) {|f| f.gets }  # Set $_ to something known
    lambda { $stdout.print }.should output_to_fd("#{string}#{$\}", STDOUT)
  end

  it "writes obj.to_s followed by $\\ (if any) to the stream when given one object" do
    o = Object.new
    def o.to_s(); 'I am an object'; end

    lambda { $stdout.print(o) }.should output("#{o.to_s}#{$\}")
  end

  it "does not call obj.to_str" do
    o = mock('o')
    o.should_not_receive(:to_str)
    o.should_receive(:to_s)
    
    require 'stringio'

    StringIO.new.print(o)
  end

  it "writes each obj.to_s to the stream and appends $\\ (if any) given multiple objects" do
    o, o2 = Object.new, Object.new
    def o.to_s(); 'o'; end
    def o2.to_s(); 'o2'; end

    lambda { $stdout.print(o, o2) }.should output("#{o.to_s}#{o2.to_s}#{$\}")
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.print("stuff") }.should raise_error(IOError)
  end
end

describe "IO#print" do
  it "needs to be reviewed for spec completeness"
end
