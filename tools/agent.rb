require 'rubygems'
require 'bert'
require 'bert/decode'
require 'socket'

class Agent
  def initialize(port)
    @socket = TCPSocket.new("localhost", port)
    p decode()
  end

  def decode
    decoder = BERT::Decode.new @socket
    decoder.read_any
  end

  def uname
    enc = BERT.encode t[:uname]
    @socket << enc
    decode
  end

  def close
    @socket << BERT.encode(:close)
    decode
  end

  def get_config(key)
    @socket << BERT.encode(t[:get_config, key.to_s])
    decode
  end

  def set_config(key, val)
    @socket << BERT.encode(t[:set_config, key.to_s, val.to_s])
    decode
  end
end

agent = Agent.new(ARGV.shift.to_i)

p agent.uname
# agent.close

p agent.get_config("gc.bytes")
