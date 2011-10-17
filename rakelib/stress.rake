namespace :stress do
  STRESS_ITERATIONS = (ENV["STRESS_ITERATIONS"] || 10).to_i

  desc "Stress test installing a list of popular gems"
  task :gems do
    gems = %w[
      amqp
      bundler
      chef
      json
      puppet
      rails
      rspec
      rspec
      unicorn
      yajl-ruby
    ]

    STRESS_ITERATIONS.times do
      gems.each do |gem|
        sh "bin/rbx -S gem install #{gem}"
      end
    end
  end
end
