extension do |e|
  e.name 'readline'
  e.files '*.c'
  e.includes '.', '..', '/usr/include'
  e.libs 'readline', 'ncurses'
end

