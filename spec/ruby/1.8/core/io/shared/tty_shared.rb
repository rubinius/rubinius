require File.dirname(__FILE__) + '/../fixtures/classes'

shared :io_tty do |cmd|
  describe "IO##{cmd}" do
    # Yeah, this will probably break.
    it "returns true if this stream is a terminal device (TTY)" do
      File.open('/dev/tty') {|f| f.send cmd }.should == true
    end

    it "returns false if this stream is not a terminal device (TTY)" do
      File.open(__FILE__) {|f| f.send cmd }.should == false
    end

    it "raises IOError on closed stream" do
      lambda { IOSpecs.closed_file.send cmd }.should raise_error(IOError)
    end
  end
end
