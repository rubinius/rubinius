require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "File#reopen" do
    before :each do
      @file = tmp('core-file-reopen.txt')
      touch(@file) { |f| f << "1234567890" }
    end

    after :each do
      rm_r @file
    end

    it "resets the stream to a new file path" do
      fh = File.new(@file)
      text = fh.read
      fh2 = fh.reopen(@file, "r")
      fh2.autoclose = false
      fh.read.should == text
      fh.close
    end

    it "accepts an object that has a #to_path method" do
      fh = File.new(@file)
      fh2 = fh.reopen(mock_to_path(@file), "r")
      fh2.autoclose = false
      fh.close
    end
  end
end
