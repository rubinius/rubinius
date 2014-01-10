require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::VM#methods_cache_resets" do
  it "returns a Fixnum" do
    Rubinius::VM.methods_cache_resets.kind_of?(Fixnum).should == true
  end
end
