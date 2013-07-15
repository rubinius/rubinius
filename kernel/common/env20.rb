# -*- encoding: us-ascii -*-

module Rubinius
  class EnvironmentVariables
    alias_method :to_h, :to_hash
  end
end
