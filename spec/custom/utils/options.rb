# Custom MSpec options
#
class MSpecOptions
  def compiler
    # The require is inside the method because this file has to be able to be
    # loaded in MRI and there are parts of the custom ensemble that are
    # Rubinius specific (primarily iseq, which could potentially be fixed by
    # better structuring the compiler).
    require 'spec/custom/runner/relates'

    on("--compiler", "Run only the compile part of the compiler specs") do
      SpecDataRelation.enable :compiler
    end

    on("--parser", "Run only the parse part of the compiler specs") do
      SpecDataRelation.enable :parser
    end

    on("--ast", "TEMPORARY: change the compiler spec matcher to use #to_ast") do
      require 'compiler-ng'
      require 'spec/custom/matchers/compile_as-ng'
    end
  end

  def gc_stats
    on("--gc-stats", "Show GC stats at the end") do
      config[:gc_stats] = true
    end
  end

  def profiler
    on("--profile", "Show profiler data at the end") do
      config[:profiler] = true
    end
  end
end
