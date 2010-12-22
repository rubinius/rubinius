require 'debugger/interface'
require 'socket'
require 'cgi'

module Rubinius
  class Debugger
    class Server < Interface
      REMOTE_DEBUG_PORT = 1098

      class ClientQuitError < RuntimeError; end

      def initialize(port=REMOTE_DEBUG_PORT, host=nil)
        @port = port
        @host = host
        Debugger.instance.interface = self
        load_commands
      end

      # Begins listening on the configured host/port for debugger commands
      def listen
        @server = @host ? TCPServer.open(@host, @port) : TCPServer.open(@port)
        @server.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, [1].pack("i"))
        at_exit { close }

        addrs = @server.addr[2..-1].uniq

        STDOUT.puts "*** Debugger listening on #{addrs.collect{|a|"#{a}:#{@port}"}.join(' ')}"

        # Wait for a debugger client to connect
        @client = @server.accept
        STDOUT.puts "*** Debugger client connected from #{@client.peeraddr[2]}:#{@client.peeraddr[1]}"
      end

      # Debugger callback implementation for getting debug commands from a user
      def process_commands(dbg, thread, ctxt, bp_list)
        begin
          until @done do
            while line = @client.gets # read a line at a time
              line.chomp!
              line.strip!
              STDOUT.puts "[#{Time.now}]: #{line}"
              output = process_command(dbg, line)
              @client.puts CGI::escape(output.to_s)
              @client.flush
              break if @done
            end

            unless line
              STDOUT.puts "*** EOF on socket - client disconnected"
              dbg.quit!
            end
          end
        rescue => e
          STDOUT.puts "*** Client disconnected"
          STDERR.puts e
          STDERR.puts e.awesome_backtrace
          dbg.quit!
        ensure
          if dbg.quit?
            close
          end
        end
      end

      # Closes the connection to the client
      def close
        if @client
          @client.puts 'finished'
          @client.flush
          @client.close # close socket on error
        end
        if @server
          @server.close
          @server = nil
          STDOUT.puts "*** Debug server finished"
        end
      end
    end
  end
end
