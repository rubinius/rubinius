module MatzBot::Commands
  
  require 'open-uri'
  require 'rexml/document'
  
  GIT_URL = 'http://git.rubini.us/?p=code;a=atom'

  hup_proc = lambda {
	  trap("HUP", "IGNORE")
	  trap("HUP", hup_proc)
  }
  trap("HUP", hup_proc)

  abrt_proc = lambda {
	  trap("ABRT", "IGNORE")
	  trap("ABRT", abrt_proc)
  }
  trap("ABRT", abrt_proc)
  
  def update_git
    data = open(GIT_URL).read
   
    doc = REXML::Document.new(data)
   
    last_hash = session[:git_last_hash]
    person = nil
    top_hash = nil
    
    REXML::XPath.each(doc, "//entry") do |entry|
      title = REXML::XPath.first(entry, "./title")
      link =  REXML::XPath.first(entry, "./link")
      name =  REXML::XPath.first(entry, "./author/name")
      hash = link.attributes['href'].split("=").last
      
      top_hash = hash if top_hash.nil?
      
      break if hash == last_hash

      # we need to put the hast already in now, otherwise it might run the build twice.
      session[:git_last_hash] = top_hash

      person = name.text
      build  = IO.popen("~/continuous/bin/rubinius.zsh #{hash}", "r+") { |p| p.read }
      unless build.empty?
	say "#{person}: #{hash[0..8]}; #{build}"
        #build.split("\n").map{|x| say "  * " << x}
      end

      break # only run it for the very last commit
    end
  end
  
  Signal.trap("USR2") do
    update_git
  end
end
