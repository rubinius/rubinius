output = "rdoc-output"
rm = "rm -rf #{output}"
cmd = "#{rm}; #{ENV['VM']} -S rdoc --all --force-update --threads=1 --output #{output} rdoc-2.4.3/lib"

# ensure everything is compiled
system cmd

Bench.run [1] do |n|
  system cmd
end

system rm
