begin
  gem 'cucumber'
  require 'cucumber/rake/task'
rescue Exception
  nil
end

if defined?(Cucumber)
  Cucumber::Rake::Task.new do |t|
    t.cucumber_opts = "--format pretty --no-source"
  end
else
  warn "Cucumber gem is required, please install it. (gem install cucumber)"
end

