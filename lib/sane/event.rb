
def otherwise(expression)
  expression
end

module IOWatcher
  def readable!
    @readable.call self
  end
end

module Events
  # Internally
  class Read
    def activate()
      # Assume
      @back.call self
    end
  end
end
#
#class IOCallBack
#  def call(event)
#    case event
#    when Read
#      event.obj.readable!
#    when Write
#      event.obj.writable!
#    when Timeout
#      event.obj.timed_out!
#    else
#      event.obj.error!
#    end
#  end
#end

class Channel

end

class ClientClass
  def wait_for_events_on(channel)
    channel.receive
  end
end


client = Server.accept

client.sequences do |client|
  synchronous :eval do end

  sequence [:eval, :print] => :mainloop

  sequence "mainloop" => :eval, :print do mainloop end
  sequence "finished" => :eval, :quit
end

# The basics
client.when_readable  {|c| c.read }
client.when_writable  {|c| c.write if data_to_write }
client.when_timed_out {|c| c.quit }

# Additional stuff
client.on_read {|c, line| quit if c =~ /quit/i or otherwise($stdout.puts line) }
client.on_quit {|c| $stdout.puts "#{c.inspect} exited." }

client.wait_for_events_on channel   # -> E.g. client.readable!


# Or could go the other way around
channel = Channel.new
channel.on_readable { ... }
channel.on_timed_out { ... }

Scheduler.notify_when_readable, channel, client


