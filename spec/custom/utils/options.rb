# Custom MSpec options
#
class MSpecOptions
  def compiler
    on("--compiler", "Run only the compile part of the compiler specs") do
      SpecDataRelation.enable :compiler
    end
  end

  def parser_19
    on("--parser-1.9", "Run the compiler specs with the 1.9 parser") do
      require 'spec/custom/matchers/compile_as_19'
      config[:parser_19] = true
    end
  end

  def memory
    on("--memory", "Display total memory in use before exiting") do
      config[:memory] = true
    end
  end

  def gc_stats
    on("--gc-stats", "Show GC stats at the end") do
      config[:gc_stats] = true
    end
  end

  def profiler
    on("--profiler", "Show profiler data at the end") do
      config[:profiler] = true
    end
  end

  def agent
    on("--agent", "Start the Rubinius agent") do
      config[:launch] << "-Xagent.start"
    end
  end
end
