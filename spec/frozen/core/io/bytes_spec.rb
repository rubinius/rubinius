# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is '1.8.7' do
  describe "IO#bytes" do
    before :each do
      @original = $KCODE
      $KCODE = "UTF-8"
      @io = File.open(IOSpecs.gets_fixtures)
    end

    after :each do
      @io.close unless @io.closed?
      $KCODE = @original
    end

    it "returns an enumerator of the next bytes from the stream" do
      enum = @io.bytes
      enum.should be_kind_of(enumerator_class)
      @io.readline.should == "Voici la ligne une.\n"
      enum.first(5).should == [81, 117, 105, 32, 195]
    end

    it "ignores a block" do
      @io.bytes{ raise "oups" }.should be_kind_of(enumerator_class)
    end

    it "raises IOError on closed stream" do
      enum = IOSpecs.closed_file.bytes
      lambda { enum.first }.should raise_error(IOError)
      enum = @io.bytes
      enum.first.should == 86
      @io.close
      lambda { enum.first }.should raise_error(IOError)      
    end
  end
end