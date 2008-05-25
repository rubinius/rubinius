extension do |e|
  e.name 'readline'
  e.files '*.c'
  e.includes '.', '..', (ENV['READLINE_DIR'] ? "#{ENV['READLINE_DIR']}/include" : '/usr/include')
  e.libs 'readline', 'ncurses'
  e.flags "-L#{ENV['READLINE_DIR']}/lib" if ENV['READLINE_DIR']
end

