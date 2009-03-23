# Custom MSpec action that prints GC stats after a spec run
#
class GCStatsAction
  def register
    MSpec.register :start,  self
    MSpec.register :finish, self
  end

  def start
    @stats = Rubinius::Stats::GC.new
    @stats.clear
  end

  def finish
    @stats.show
  end
end
