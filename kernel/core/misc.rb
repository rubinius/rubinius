def self.to_s
  "main"
end

module Rubinius
  AtExit = []
end

class NilClass
  alias_method :|, :^
end

class TrueClass
  alias_method :inspect, :to_s
end

class FalseClass
  alias_method :|, :^
  alias_method :inspect, :to_s
end
