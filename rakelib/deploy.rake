desc "Deploy a Rubinius release"
task :deploy do
  sh "./scripts/deploy.sh"
end
