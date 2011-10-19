require 'rubinius/agent'

module Rubinius
  # The Agent provides both a low-level communication interface to the VM and
  # a variety of atomic or semi-atomic data elements about the VM's
  # functioning. It is valuable to keep the Agent simple.
  #
  # There are common questions that could be answered from the data provided
  # by the Agent. These are essentially analysis questions. This class
  # provides an API for getting that data.
  #
  # A third level would be an application, such as a web interface or IDE
  # plugin, that uses the Analyst API to render data visually.

  class Analyst
    include Rubinius::Stats::Units

    def initialize
      @agent = Rubinius::Agent.loopback
    end

    MEMORY_BANKS = [:young, :mature, :large, :code, :symbols]

    def total_memory
      total = MEMORY_BANKS.inject(0) do |s, m|
        s + @agent.get("system.memory.#{m}.bytes").last
      end

      auto_bytes total
    end

    def itemized_memory
      total = 0
      output = ""
      MEMORY_BANKS.each do |m|
        size = @agent.get("system.memory.#{m}.bytes").last
        output += "#{m.to_s.capitalize.rjust(7)}: #{auto_bytes size}\n"
        total += size
      end
      output += "\n  Total: #{auto_bytes total}\n"

      output
    end
  end
end
