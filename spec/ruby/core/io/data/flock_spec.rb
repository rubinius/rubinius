require File.expand_path('../../../../spec_helper', __FILE__)

describe "DATA.flock" do
  before :each do
    @data2_name = fixture __FILE__, "data2.rb"
  end

  platform_is_not :windows do
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
end