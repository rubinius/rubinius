require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#each_byte" do
  it "raises IOError on closed stream" do
    # each_byte must have a block in order to raise the Error.
    # MRI 1.8.7 returns enumerator if block is not provided.
    # See [ruby-core:16557].
    lambda { IOSpecs.closed_file.each_byte {} }.should raise_error(IOError)
  end

  it "yields each byte" do
    open IOSpecs.gets_fixtures do |io|
      bytes = []

      io.each_byte do |byte|
        bytes << byte
        break if bytes.length >= 5
      end

      bytes.should == [86, 111, 105, 99, 105]
    end
  end

  it "works on empty streams" do
    f = File.new(tmp("io-each-byte-spec"), "w+") 
    lambda { 
      f.each_byte { |b| raise IOError }
    }.should_not raise_error
    f.close
    File.unlink(tmp("io-each-byte-spec"))
  end
end
