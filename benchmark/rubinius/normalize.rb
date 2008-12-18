#!/usr/bin/env ruby
#
# Expects to parse YAML output like
#
#  String#===:
#    MRI: 0.01234
#    RBX: 0.12345
#    ratio: 10.000
#
# Also expects an entry 'base'. Normalizes the times by reducing 
# by base and recalculates the ratio. Sorts descending by ratio.
#
# Obviously, this 'normalize' is only an approximation, since the
# time recorded for the 'base' loop is not necessarily the same
# as the percentage of time taken by the loop in each benchmark.
# However, normalizing the time provides a much more accurate
# ratio, so the benefit is real.
#
# For example:
#   benchmark/rubinius/run_benches | benchmark/rubinius/normalize.rb

require 'yaml'

# results
result = YAML.load(STDIN)

# normalize
base_mri = result['base']['MRI']
base_rbx = result['base']['RBX']
result.each do |k,v|
  next if k == 'base'
  v['MRI'] -= base_mri
  v['RBX'] -= base_rbx
  v['ratio'] = v['RBX'] / v['MRI']
end

# descending ratio
result = result.sort { |a, b| b[1]['ratio'] <=> a[1]['ratio'] }

# output
format = "%-30s%12s%12s%15s"
puts format % ['Method', 'MRI', 'RBX', 'Ratio']
result.each do |k,v|
  puts format % [k, "%.6f" % v['MRI'], "%.6f" % v['RBX'], "%.6f" % v['ratio']]
end
