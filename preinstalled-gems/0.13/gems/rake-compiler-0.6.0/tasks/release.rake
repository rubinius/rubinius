begin
  gem 'rubyforge', '~> 1.0.1'
  require 'rubyforge'
rescue Exception
  nil
end

if defined?(RubyForge) then
  if defined?(GEM_SPEC) then
    desc 'Package and upload to RubyForge'
    task :release => [:clobber, :package] do |t|
      ver = ENV['VERSION'] or fail "Must supply VERSION (rake release VERSION=x.y.z)."

      # compare versions to avoid mistakes
      unless ver == GEM_SPEC.version.to_s then
        fail "Version mismatch (supplied and specification versions differ)."
      end

      # no rubyforge project? no release for you!
      if GEM_SPEC.rubyforge_project == 'TODO' or GEM_SPEC.rubyforge_project.nil? then
        fail "Must define rubyforge_project in your gem specification."
      end

      # instantiate a RubyForge object
      rf = RubyForge.new

      # read project info and overview
      notes = begin
                r = File.read("README.rdoc")
                r.split(/^(=+ .*)/)[1..4].join.strip
              rescue
                warn "Missing README.rdoc"
                ''
              end

      # read changes
      changes = begin
                  h = File.read("History.txt")
                  h.split(/^(===+ .*)/)[1..2].join.strip
                rescue
                  warn "Missing History.txt"
                  ''
                end

      # build the configuration for the release
      config = Hash.new
      config["release_notes"] = notes
      config["release_changes"] = changes
      config["preformatted"] = true

      # prepare configuration
      rf.configure config

      files = FileList["pkg/#{GEM_SPEC.name}-#{GEM_SPEC.version}*.*"].to_a
      fail "No files found for the release." if files.empty?

      puts "Logging in RubyForge..."
      rf.login

      puts "Files to upload:"
      files.each do |f|
        puts "  * #{f}"
      end

      puts "Releasing #{GEM_SPEC.name} version #{GEM_SPEC.version}..."
      rf.add_release GEM_SPEC.rubyforge_project, GEM_SPEC.name, GEM_SPEC.version, *files
      puts "Done."
    end
  else
    warn "no GEM_SPEC is found or defined. 'release' task cannot work without it."
  end
else
  warn "rubyforge gem is required to generate releases, please install it (gem install rubyforge)."
end
