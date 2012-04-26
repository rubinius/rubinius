# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is '1.9.3' do
  describe "IO#advise" do
    before :each do
      @kcode, $KCODE = $KCODE, "utf-8"
      @io = IOSpecs.io_fixture "lines.txt"
    end

    after :each do
      @io.close unless @io.closed?
      $KCODE = @kcode
    end

    it "needs to be reviewed for spec completeness"

    it "raises a TypeError if advise is not a Symbol" do
      lambda{
        @io.advise("normal")
      }.should raise_error(TypeError)
    end

    it "raises a NotImplementedError if advise is not recognized" do
      lambda{
        @io.advise(:foo)
      }.should raise_error(NotImplementedError)
    end

    it "returns nil when invoked with correct arguments" do
      @io.advise(:normal).should == nil
    end
  end
end
