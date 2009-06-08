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

  def windows_env_echo(var)
    `cmd.exe /C ECHO %#{var}%`.strip
  end  

  def username
    user = ""
    if SpecGuard.windows?
      user = windows_env_echo('USERNAME')
    else
      user = `whoami`.strip
    end
    user
  end

  def home_directory
    return ENV['HOME'] unless SpecGuard.windows?
    windows_env_echo('HOMEDRIVE') + windows_env_echo('HOMEPATH')
  end  
end
