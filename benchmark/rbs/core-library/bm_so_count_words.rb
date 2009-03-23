# $Id: wc-ruby.code,v 1.4 2004/11/13 07:43:32 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# with help from Paul Brannan

def concatenate(n)
  n.times do
    input = open(File.dirname(__FILE__) + '/wc.input', 'rb')

    nl = nw = nc = 0
    while true
      data = (input.read(4096) or break) << (input.gets || "")
      nc += data.length
      nl += data.count("\n")
      ((data.strip! || data).tr!("\n", " ") || data).squeeze!
      nw += data.count(" ") + 1
    end

    input.close
    puts "#{nl} #{nw} #{nc}"
  end
end

Bench.run [100] do |n|
  n.times { concatenate(500) }
end
