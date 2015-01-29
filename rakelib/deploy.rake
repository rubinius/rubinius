desc "Deploy a Rubinius release"
task :deploy do
  puts "Deploying #{ENV["TRAVIS_TAG"]}..."
end
