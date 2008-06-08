shared :socketpair do |cmd|
  not_supported_on :jruby, :windows do
    describe "Socket##{cmd}" do
      it "ensures the returned sockets are connected" do
        s1, s2 = Socket.socketpair(Socket::AF_UNIX, 1, 0)
        s1.puts("test")
        s2.gets.should == "test\n"
      end
    end
  end
end
