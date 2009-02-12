require File.dirname(__FILE__) + '/../../../spec_helper'

describe "DATA.flock" do
  before :each do
    @data2_name = fixture __FILE__, "data2.rb"
  end

  it "succeeds in locking the file DATA came from" do
    data = ruby_exe(@data2_name)
    data.should == "0"

    begin
      file = File.open(@data2_name)
      file.flock(File::LOCK_EX)
      data = ruby_exe(@data2_name)
      data.should == "false"
    ensure
      file.flock(File::LOCK_UN) if file
    end
  end
end