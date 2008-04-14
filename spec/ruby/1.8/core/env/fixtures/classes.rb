module EnvSpecs
  def self.get_env
    platform_is_not :mswin do
      # return is needed otherwise nil is returned from get_env
      return Hash[*`env`.split("\n").map { |e| e.split("=", 2) }.flatten]
    end
  end
end
