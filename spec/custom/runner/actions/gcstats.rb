# Custom MSpec action that prints GC stats after a spec run
#
class GCStatsAction
  SEC_PER_NS = 1.0e-9
  MS_PER_NS  = 1.0e-6

  def register
    MSpec.register :start,  self
    MSpec.register :finish, self
  end

  def start
    @timing = Rubinius::VM.gc_info
  end

  def elapsed(time)
    elapsed = time * SEC_PER_NS
    return "%.1fs" % elapsed if elapsed > 1.0

    elapsed = time * MS_PER_NS
    "%dms" % elapsed
  end

  def finish
    timing = Rubinius::VM.gc_info - @timing
    stats = "\nTime spent in GC: %s" % elapsed(timing)

    if formatter = MSpec.retrieve(:formatter)
      percentage = timing * SEC_PER_NS * 100.0 / formatter.timer.elapsed
      stats << " (%.1f%%)" % percentage
    end

    puts stats, "\n"
  end
end
