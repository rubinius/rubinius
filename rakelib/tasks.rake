class Ticket

  def self.user(id)
    @users ||= Hash.new do |h,k|
      str = %Q!curl -s -H 'X-LighthouseToken: c2d8226589bdc2e23d025dc39586459ab5da0349' http://rubinius.lighthouseapp.com/users/#{k}.xml!
      if m = %r!<name>(.*)</name>!.match(`#{str}`)
        m[1]
      else
        "user#{k}"
      end
    end

    @users[id]
  end
end

# Rake tasks for dealing with programming tasks
namespace :tasks do

  desc "Print out the current tasks being worked on from Lighthouse"
  task :current do
    require 'hpricot'
    str = %q!curl -s -H 'X-LighthouseToken: c2d8226589bdc2e23d025dc39586459ab5da0349' http://rubinius.lighthouseapp.com/projects/5089/tickets.xml\?q="tagged:current"!

    h = Hpricot.parse(`#{str}`)

    (h / 'ticket').each do |e|
      uid = (e / 'assigned-user-id').text.to_i
      user = Ticket.user(uid)
      tags = (e / 'tag').text
      title = (e / 'title').text

      puts "== #{title}"
      puts "     #{user} - #{tags.split(" ").join(", ")}"
    end
  end
end
