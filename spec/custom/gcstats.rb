# Custom MSpec action that prints GC stats after a spec run
#
class GCStatsAction
  def register
    MSpec.register :finish, self
  end

  def finish
    timing = Rubinius::VM.gc_info
    puts "\nTime spent in GC: #{timing / 1000000}ms"
  end
end
