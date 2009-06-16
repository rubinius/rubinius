dir = "rdoc-gem-remote"
rm  = "rm -rf #{dir}"
cmd = "#{rm}; #{ENV['VM']} -S gem install --remote --install-dir #{dir} --bindir #{dir} --no-rdoc --no-ri --ignore-dependencies rdoc --version '=2.4.3'"

# ensure everything is compiled
system cmd

Bench.run [1] do |n|
  system cmd
end

system rm
