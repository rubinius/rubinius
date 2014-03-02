require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::VM#global_serial" do
  it "returns a Fixnum" do
    Rubinius::VM.global_serial.kind_of?(Fixnum).should == true
  end
end
