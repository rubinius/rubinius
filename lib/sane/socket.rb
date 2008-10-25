module Sane

  class Socket; end

  sock = Socket.new

  sock.when_ready { connect_to "127.0.0.1", 8080 }
  sock.when_connected { send "Hi" and read response }
  sock.when_
  sock.when_sent { disconnect }

  module Connected; end

  module TCP; end


  # Server

  server = Server.new
  server.extend TCP

  server.on_receiving_signal(SIGINTR) {
    instruct clients { :quit }
  }

  server.
   


  def self.new(); end

  def when_signalled(); end

  def when_created(); end
  def when_bound(); end
  def when_ready_for_connections(); end

  def when_accepted_a_new_connection(); end

  def when_setup_fails(); end
  def when_an_error_occurs(); end


  # Client

  attr_accessor :descriptor

  def when_created(); end

  def when_needing_a_buffer(); end
  def when_instructed_by_server_to(); end

  def when_done(); end
    def when_disconnected(); end
    def when_closed(); end

  def when_connection_broken(); end


  # Agent

  def self.new(remote); end

  def on_connection(); end
  def on_failed_connect(); end

  # Messaging
  def on_receive(); end
  def on_write(); end     # Pointless?

  # Finishing
  def on_broken_connection(); end
  def on_disconnection(); end

  # System level
  def on_create(); end
  def on_close(); end

  end


end
