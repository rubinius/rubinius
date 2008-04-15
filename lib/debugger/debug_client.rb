require 'socket'
require 'readline'
require 'cgi'

host = ARGV[0] || 'localhost'
port = ARGV[1] || 1098

socket = TCPSocket.new(host, port)
puts "Connected to debugger on #{host}:#{port}"
prompt = "\nrbx:debug> "
while true
  readers, _, _ = IO.select([socket], nil, nil, 0.1)
  if readers
    response = CGI::unescape(socket.gets)
    break if !response or response == "finished\n"
    puts response
  end
  line = Readline.readline(prompt)
  socket.puts line
  break if line =~ /exit/
  response = CGI::unescape(socket.gets)
  break if !response or response == "finished\n"
  puts response
end
socket.close

