require 'rubygems'
require 'erb'
require 'redcloth'

section_template = ERB.new(File.read('section_template.html.erb'),nil,'<>')

ARGV.each do |file|
  begin
    html_file = file.chomp(File.extname(file)) << '.html'
    markup = RedCloth.new File.read(file)
    File.open(html_file, 'w') do |f|
      html = markup.to_html
      f.puts section_template.result(binding)
    end
  rescue Exception => ex
    puts "An exception occurred while processing section file '#{file}'"
    raise ex
  end
end
