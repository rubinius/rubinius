# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is '1.8.7' do
  describe "IO#bytes" do
    before :each do
      @kcode, $KCODE = $KCODE, "utf-8"
      @io = IOSpecs.io_fixture "lines.txt"
    end

    after :each do
      @io.close unless @io.closed?
      $KCODE = @kcode
    end

    it "returns an enumerator of the next bytes from the stream" do
      enum = @io.bytes
      enum.should be_an_instance_of(enumerator_class)
      @io.readline.should == "Voici la ligne une.\n"
      enum.first(5).should == [81, 117, 105, 32, 195]
    end

    it "does not yield to a block" do
      @io.bytes{ flunk }.should be_an_instance_of(enumerator_class)
    end

    it "raises an IOError on closed stream" do
      enum = IOSpecs.closed_file.bytes
      lambda { enum.first }.should raise_error(IOError)
    end

    it "raises an IOError on an enumerator for a stream that has been closed" do
      enum = @io.bytes
      enum.first.should == 86
      @io.close
      lambda { enum.first }.should raise_error(IOError)
    end
  end
end
