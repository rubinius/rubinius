begin
  gem 'rubyforge', '~> 1.0.1'
  require 'rubyforge'
rescue Exception
  nil
end

CLEAN.include('email.txt')

if defined?(RubyForge) then
  if defined?(GEM_SPEC) then
    desc 'Create news email file and post to RubyForge.'
    task :announce do |t|
      ver = ENV['VERSION'] or fail "Must supply VERSION (rake announce VERSION=x.y.z)."

      # compare versions to avoid mistakes
      unless ver == GEM_SPEC.version.to_s then
        fail "Version mismatch (supplied and specification versions differ)."
      end

      # no homepage? why announce it then?!
      if GEM_SPEC.homepage == 'TODO' or GEM_SPEC.homepage.nil? then
        fail "Must define homepage in your gem specification."
      end

      # no rubyforge project? no release for you!
      if GEM_SPEC.rubyforge_project == 'TODO' or GEM_SPEC.rubyforge_project.nil? then
        fail "Must define rubyforge_project in your gem specification."
      end

      # instantiate a RubyForge object
      rf = RubyForge.new.configure

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

      # standard fields
      subject = "#{GEM_SPEC.name} #{GEM_SPEC.version} Released"
      title   = "#{GEM_SPEC.name} version #{GEM_SPEC.version} has been released!"
      body    = "#{notes}\n\nChanges:\n\n#{changes}"
      urls    = [GEM_SPEC.homepage, "http://rubyforge.org/projects/#{GEM_SPEC.rubyforge_project}"].map { |u| "* <#{u.strip}>" }.join("\n")

      puts "Logging in RubyForge..."
      rf.login

      puts "Generating email.txt..."
      File.open("email.txt", "w") do |mail|
        mail.puts "Subject: [ANN] #{subject}"
        mail.puts
        mail.puts title
        mail.puts 
        mail.puts urls
        mail.puts
        mail.puts body
      end
      puts "Created email.txt"

      puts "Posting news for #{GEM_SPEC.name} version #{GEM_SPEC.version}..."
      rf.post_news GEM_SPEC.rubyforge_project, subject, "#{title}\n\n#{body}"
      puts "Done."
    end
  else
    warn "no GEM_SPEC is found or defined. 'announce' task cannot work without it."
  end
else
  warn "rubyforge gem is required to generate announces, please install it (gem install rubyforge)."
end
