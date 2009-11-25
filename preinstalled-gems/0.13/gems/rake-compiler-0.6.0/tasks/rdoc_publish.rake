begin
  gem 'rubyforge', '~> 1.0.1'
  require 'rubyforge'
rescue Exception
  nil
end

if defined?(RubyForge) then
  if defined?(DOC) && defined?(GEM_SPEC) then
    desc "Publish RDoc to RubyForge"
    task :publish => [:clobber_rdoc, :rdoc] do
      config_file = File.expand_path('~/.rubyforge/user-config.yml')
      fail "You need rubyforge properly configured." unless File.exist?(config_file)

      # no rubyforge project? no release for you!
      if GEM_SPEC.rubyforge_project == 'TODO' or GEM_SPEC.rubyforge_project.nil? then
        fail "Must define rubyforge_project in your gem specification."
      end

      # use YAML to load configuration file
      config = YAML.load_file(config_file)

      host = "#{config['username']}@rubyforge.org"
      remote_dir = "/var/www/gforge-projects/#{GEM_SPEC.rubyforge_project}/"
      local_dir = DOC.rdoc_dir

      # use PuTTY pscp or scp on other platforms
      ssh_exe = RUBY_PLATFORM =~ /mswin|mingw/ ? 'pscp' : 'scp'

      # construct the command
      cmd = [ssh_exe]
      cmd << '-r' << '-q'  # recursive and quiet options
      cmd << "#{local_dir}/*"
      cmd << "#{host}:#{remote_dir}"

      puts "Publishing RDocs to RubyForge..."
      sh cmd.join(' ')
    end
  else
    warn "You need a GEM_SPEC and DOC rdoc definitions present. task publish not defined."
  end
else
  warn "rubyforge gem is required to generate releases, please install it (gem install rubyforge)."
end
