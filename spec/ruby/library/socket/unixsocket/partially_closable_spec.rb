require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require File.dirname(__FILE__) + '/../shared/partially_closable_sockets'

platform_is_not :windows do
  describe "UNIXSocket partial closability" do
    
    before :each do
      @path = SocketSpecs.socket_path
      File.unlink(@path) if File.exists?(@path)
      @server = UNIXServer.open(@path)
      @s1 = UNIXSocket.new(@path)
      @s2 = @server.accept
    end

    after :each do
      @server.close
      @s1.close
      @s2.close
      File.unlink(@path) if File.exists?(@path)
    end
    
    it_should_behave_like "partially closable sockets"
    
  end
end