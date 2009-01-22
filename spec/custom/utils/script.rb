require 'spec/custom/utils/options'
require 'spec/custom/runner/actions/gcstats'

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
    options.compiler
    options.gc_stats
  end
end

# Custom options for mspec-ci
#
class MSpecCI
  def custom_options(options)
    options.compiler
    options.gc_stats
  end
end

# Custom options for mspec-tag
#
class MSpecTag
  def custom_options(options)
    options.compiler
  end
end
