require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#ioctl" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.ioctl(5, 5) }.should raise_error(IOError)
  end
  
  platform_is :os => :linux do
    it "replaces a String's content with the output buffer" do
      File.open('/dev/tty', 'r') do |f|
        # Process group ID and session ID.
        # Ruby has Process.getpgrp but no method to get the session ID.
        ids = `ps -j --no-heading -p #{Process.pid}`.split[1, 2].map(&:to_i)
        buffer = ''
        # TIOCGPGRP in /usr/include/asm-generic/ioctls.h
        f.ioctl 0x540F, buffer
        ids.should include(buffer.unpack('I').first)
      end
    end
    
    it "raises an Errno error when ioctl fails" do
      File.open('/dev/tty', 'r') do |f|
        lambda {
          # TIOCGPGRP in /usr/include/asm-generic/ioctls.h
          f.ioctl 0x540F, nil
        }.should raise_error(Errno::EFAULT)
      end
    end
  end
end
