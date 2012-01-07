# -*- encoding: us-ascii -*-

module Rubinius
  class Alias
    attr_reader :original_exec
    attr_reader :original_module
    attr_reader :original_name
  end
end
