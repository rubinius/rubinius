module EnvSpecs
  def self.get_env
    env = ""
    platform_is_not :windows do
      env = Hash[*`env`.split("\n").map { |e| e.split("=", 2) }.flatten]
    end
    env
  end

  def self.get_current_user
    user = ""
    platform_is_not :windows do
      user = `whoami`.strip
    end
    user
  end
end
