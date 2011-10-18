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
      sinatra
      unicorn
      yajl-ruby
    ]

    gem_home = File.expand_path "stress_gems_install"

    begin
      STRESS_ITERATIONS.times do
        gems.each do |gem|
          FileUtils.rm_rf gem_home
          sh "bin/rbx -S gem install -i #{gem_home} #{gem}"
        end
      end
    ensure
      FileUtils.rm_rf gem_home
    end
  end
end
