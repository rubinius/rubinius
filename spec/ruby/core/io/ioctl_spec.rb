require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#ioctl" do
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.ioctl(5, 5) }.should raise_error(IOError)
  end
  
  platform_is :os => :linux do
    it "resizes an empty String to match the output size" do
      File.open(__FILE__, 'r') do |f|
        buffer = ''
        # FIONREAD in /usr/include/asm-generic/ioctls.h
        f.ioctl 0x541B, buffer
        buffer.unpack('I').first.should be_kind_of(Integer)
      end
    end
    
    it "passes data from and to a String correctly" do
      s = Socket.new Socket::AF_INET, Socket::SOCK_DGRAM, 0
      # /usr/include/net/if.h, structure ifreq
      # The structure is 32 bytes on x86, 40 bytes on x86_64
      if_name = ['lo'].pack('a16')
      buffer = if_name + 'z' * 24
      # SIOCGIFADDR in /usr/include/bits/ioctls.h
      s.ioctl 0x8915, buffer
      s.close
      
      # Interface name should remain unchanged.
      buffer[0, 16].should == if_name
      # lo should have an IPv4 address of 127.0.0.1
      buffer[16, 1].unpack('C').first.should == Socket::AF_INET
      buffer[20, 4].should == "\x7f\0\0\x01"
    end
    
    it "raises an Errno error when ioctl fails" do
      File.open(__FILE__, 'r') do |f|
        lambda {
          # TIOCGWINSZ in /usr/include/asm-generic/ioctls.h
          f.ioctl 0x5413, nil
        }.should raise_error(Errno::ENOTTY)
      end
    end
  end
end
