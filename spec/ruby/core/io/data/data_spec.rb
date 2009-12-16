require File.dirname(__FILE__) + '/../../../spec_helper'

describe "DATA" do
  before :each do
    @data1_name = fixture __FILE__, "data1.rb"
  end

  it "presents $0 file data after __END__ as a File object" do
    data = ruby_exe(@data1_name)
    data.should == "[File, \"hello\"]"
  end
end