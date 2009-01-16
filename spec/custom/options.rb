# Custom MSpec options
#
class MSpecOptions
  def gc_stats
    on("--gc-stats", "Show GC stats at the end") do
      config[:gc_stats] = true
    end
  end
end
