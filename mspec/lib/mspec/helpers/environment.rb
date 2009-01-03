require 'mspec/guards/guard'

class Object
  def env
    env = ""
    if SpecGuard.windows?
      env = Hash[*`cmd.exe /C set`.split("\n").map { |e| e.split("=", 2) }.flatten]
    else
      env = Hash[*`env`.split("\n").map { |e| e.split("=", 2) }.flatten]
    end
    env
  end

  def username
    user = ""
    if SpecGuard.windows?
      user = `cmd.exe /C ECHO %USERNAME%`.strip
    else
      user = `whoami`.strip
    end
    user
  end
end
