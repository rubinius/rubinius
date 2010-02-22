# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is '1.8.7' do
  describe "IO#chars" do
    before :each do
      @kcode, $KCODE = $KCODE, "utf-8"
      @io = IOSpecs.io_fixture "lines.txt"
    end

    after :each do
      $KCODE = @kcode
      @io.close unless @io.closed?
    end

    it "returns an enumerator of the next chars from the stream" do
      enum = @io.chars
      enum.should be_an_instance_of(enumerator_class)
      @io.readline.should == "Voici la ligne une.\n"
      enum.first(5).should == ["Q", "u", "i", " ", "è"]
    end

    ruby_version_is '1.9' do
      it "ignores a block" do
        @io.chars{ raise "oups" }.should be_an_instance_of(enumerator_class)
      end
    end

    it "returns an enumerator for a closed stream" do
      IOSpecs.closed_file.chars.should be_an_instance_of(enumerator_class)
    end

    it "raises an IOError when an enumerator created on a closed stream is accessed" do
      lambda { IOSpecs.closed_file.chars.first }.should raise_error(IOError)
    end

    it "raises an IOError when the stream for the enumerator is closed" do
      enum = @io.chars
      enum.first.should == "V"
      @io.close
      lambda { enum.first }.should raise_error(IOError)
    end
  end
end
