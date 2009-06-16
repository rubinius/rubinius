cmd = "#{ENV['VM']} -S rails -h"

# ensure everything is compiled
system cmd

Bench.run [1] do |n|
  system cmd
end
