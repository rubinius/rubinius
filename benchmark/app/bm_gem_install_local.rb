dir = "rdoc-gem-local"
rm  = "rm -rf #{dir}"
cmd = "#{rm}; #{ENV['VM']} -S gem install --local --install-dir #{dir} --bindir #{dir} --no-rdoc --no-ri --ignore-dependencies rdoc-2.4.3.gem"

# ensure everything is compiled
system cmd

Bench.run [1] do |n|
  system cmd
end

system rm
