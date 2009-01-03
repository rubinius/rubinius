require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

platform_is_not :windows do
  describe "UNIXServer#accept" do
    before :each do
      FileUtils.rm(SocketSpecs.socket_path, :force => true)
    end

    after :all do
      FileUtils.rm(SocketSpecs.socket_path, :force => true)
    end

    it "accepts what is written by the client" do
      server = UNIXServer.open(SocketSpecs.socket_path)
      client = UNIXSocket.open(SocketSpecs.socket_path)

      client.send('hello', 0)

      sock = server.accept
      data, info = sock.recvfrom(5)

      data.should == 'hello'

      server.close
      client.close
      sock.close
    end

    it "can be interrupted by Thread#kill" do
      server = UNIXServer.new(SocketSpecs.socket_path)
      t = Thread.new {
        server.accept
      }
      Thread.pass until t.status == "sleep"

      # kill thread, ensure it dies in a reasonable amount of time
      t.kill
      a = 1
      while a < 1000
        break unless t.alive?
        Thread.pass
        a += 1
      end
      a.should < 1000
      server.close
    end

    it "can be interrupted by Thread#raise" do
      server = UNIXServer.new(SocketSpecs.socket_path)
      t = Thread.new {
        server.accept
      }
      Thread.pass until t.status == "sleep"

      # raise in thread, ensure the raise happens
      ex = Exception.new
      t.raise ex
      lambda { t.join }.should raise_error(Exception)
      server.close
    end
  end
end
