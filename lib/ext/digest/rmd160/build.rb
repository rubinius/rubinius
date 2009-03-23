extension do |e|
  e.name 'rmd160'
  e.files '*.c'
  e.includes '.', '..'
  e.libs 'crypto'
end
