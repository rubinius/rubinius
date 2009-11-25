begin
  require 'spec/rake/spectask'

  begin
    require 'rcov'
  rescue LoadError
    warn "RCov gem is required, please install it (gem install rcov)."
  end

rescue LoadError
  warn "RSpec gem is required, please install it (gem install rspec)."
end

if defined?(Spec)
  Spec::Rake::SpecTask.new(:spec) do |t|
    t.spec_opts   = ["--options", "spec/spec.opts"]
    t.spec_files  = FileList["spec/**/*_spec.rb"]
  end

  if defined?(Rcov)
    CLOBBER.include('coverage')

    namespace :spec do
      desc "Run all specs in spec directory with RCov"
      Spec::Rake::SpecTask.new(:rcov) do |t|
        t.spec_opts   = ["--options", "spec/spec.opts"]
        t.spec_files  = FileList["spec/**/*_spec.rb"]
        t.rcov        = true
        t.rcov_opts   = ["--exclude", "spec/*,features/*,gems/*"]
      end
    end
  end
end

