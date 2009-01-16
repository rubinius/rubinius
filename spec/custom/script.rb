require 'spec/custom/options'
require 'spec/custom/gcstats'

# Registers custom actions, etc. for all MSpec scripts
#
class MSpecScript
  def custom_register
    GCStatsAction.new.register if config[:gc_stats]
  end
end

# Custom options for mspec-run
#
class MSpecRun
  def custom_options(options)
    options.gc_stats
  end
end

# Custom options for mspec-ci
#
class MSpecCI
  def custom_options(options)
    options.gc_stats
  end
end
