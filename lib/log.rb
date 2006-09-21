require 'logger'

class Rubinius::Logger < Logger
  def format_message(severity, time, name, msg)
    "[#{time.strftime("%H:%M:%S")} #{severity[0,1].upcase}] #{msg}\n"
  end
end

Log = Rubinius::Logger.new(STDOUT)



if $DEBUG or ENV['RDEBUG']
  Log.level = Logger::DEBUG
elsif ENV['TESTING']
  Log.level = Logger::FATAL
else
  Log.level = Logger::INFO
end