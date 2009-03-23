extension do |e|
  e.name 'sha1'
  e.files '*.c'
  e.includes '.', '..'
  e.libs 'crypto'
end
