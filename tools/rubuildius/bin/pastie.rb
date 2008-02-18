#!/usr/bin/env ruby
#
### Pastie from the console (thanks pinupgeek)
#
# file = pastie.rb (depends on pastie_client.rb)

require 'pastie_client'
require 'optparse'

class CliPastie
  
  attr :parser
  attr :opt
    
  def initialize
    @opt = {
      :parser   => "ruby",
      :nick     => "rubuildius",
      :key      => nil,
      :help     => false
    }
    
    @parser = OptionParser.new do |cmd|      
      cmd.banner += " <filename>\n\nReads from STDIN if not passed a filename.\n\n"
      cmd.on('-h', '--help', 'Show usage') { puts parser.help; exit }
#      cmd.on('-u', '--nick NICK', 'Set nickname') { |n| opt[:nick] = n }
#      cmd.on('-k', '--key KEY', 'Key to authenticate your nick') { |n| opt[:nick] = n }
      cmd.on('-l', '--language LANG', 'Set language for syntax highlighting') { |l| opt[:parser] = l }
    end
  end
  
  def run
    parser.parse!(ARGV)
    if not ARGV.empty?
      body = File.read(ARGV[0])
    else
      body=STDIN.read
    end
    return if body.strip.empty?

    p = PastieClient.new( :nick => opt[:nick], :key => opt[:key] )
    id = p.paste(body, opt[:parser])
    puts "http://pastie.caboo.se/paste/#{id}"
  end

end

if __FILE__ == $0
  CliPastie.new.run
end

