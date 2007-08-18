def self.to_s
  "main"
end

module Rubinius
  AtExit = []
end

class NilClass
  alias :| :^
end

class TrueClass
  alias :inspect :to_s
end

class FalseClass
  alias :| :^
  alias :inspect :to_s
end
