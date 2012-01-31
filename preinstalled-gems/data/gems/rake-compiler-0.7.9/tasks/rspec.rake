begin
  require "rspec/core/rake_task"

  begin
    require 'rcov'
  rescue LoadError
    warn "RCov gem is required, please install it (gem install rcov)."
  end

rescue LoadError
  warn "RSpec gem is required, please install it (gem install rspec)."
end

if defined?(RSpec::Core::RakeTask)
  RSpec::Core::RakeTask.new(:spec)

  if defined?(Rcov)
    CLOBBER.include('coverage')

    namespace :spec do
      desc "Run all specs in spec directory with RCov"
      RSpec::Core::RakeTask.new(:rcov) do |t|
        t.rcov = true
        t.rcov_opts   = ["--exclude", "spec/*,features/*,gems/*"]
      end
    end
  end
end

