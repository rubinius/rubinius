cmd = "cd ../..; #{ENV['VM']} mspec/bin/mspec-ci spec/ruby/core"

# ensure everything is compiled
system cmd

Bench.run [1] do |n|
  system cmd
end
