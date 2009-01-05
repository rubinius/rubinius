require 'profiler'

at_exit do
  Profiler__.print_profile(STDERR)
end
Profiler__.start_profile
