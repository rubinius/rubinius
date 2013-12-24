# Built-in documentation site, server, and browser launcher.
#
# A static documentation site is generated from documentation
# source files. The static site is served by Rubinius using
# the WEBrick server in the Ruby standard library. Code from
# the launchy gem has been adapted to launch a web browser to
# view the documentation.

module Rubinius
  class Documentation
    def self.main
      docs = new
      docs.options
      docs.show
    end

    def initialize
      @server_only = false
      @port = nil
      @browser = nil
      @running = false
      @documents = nil
      @url = nil
    end

    def port
      @port ||= 35421
    end

    def documents
      @documents ||= File.expand_path("../documentation", __FILE__)
    end

    def url
      @url ||= "http://localhost:#{port}/doc/en/"
    end

    def browser
      return if @server_only

      sleep 0.1 until @running

      unless @browser
        case RbConfig::CONFIG['host_os']
        when /darwin/
          @browser = "open"
        when /linux/
          browsers = %w[ xdg-open exo-open gnome-open kfmclient htmlview
                         firefox opera mozilla galeon seamonkey ]
          @browser = browsers.find { |x| `which #{x}`; x if $?.success? }
        else
          STDERR.puts "Unknown system #{RbConfig::CONFIG['host_os']}"
          STDERR.puts "Use --browser option to specify the browser to run"
        end
      end

      return unless @browser

      Thread.new do
        command = "#{@browser} #{url}"
        system command

        unless $?.success?
          STDERR.puts "\nUnable to start browser: #{command}"
          STDERR.puts "You can view the documentation by visiting #{url} in your browser"
        end
      end
    end

    def start_hook
      lambda { @running = true }
    end

    def server
      Thread.new do
        puts "Starting Rubinius Documentation Server..."
        puts "Type ^C to exit\n\n"

        server = WEBrick::HTTPServer.new(:Port => port,
                                         :DocumentRoot => documents,
                                         :StartCallback => start_hook)

        [:INT, :TERM].each do |signal|
          trap(signal) { server.shutdown }
        end

        server.start
      end
    end

    def options(argv=ARGV)
      options = Rubinius::Options.new "Usage: rbx docs [options]", 23

      options.on "-p", "--port", "NUM", "Listen on port NUM" do |port|
        @port = port
      end

      options.on "-s", "--server", "Only start server, not browser" do
        @server_only = true
      end

      options.on("-b", "--browser", "CMD",
                 "Start browser using CMD passing the URL") do |browser|
        @browser = browser
      end

      options.help
      options.doc ""

      options.parse argv
    end

    def show
      s = server
      b = browser
      b.join if b
      s.join
    end
  end
end
