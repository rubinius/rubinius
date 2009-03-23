extension do |e|
  e.name 'md5'
  e.files '*.c'
  e.includes '.', '..'
  e.libs 'crypto'
end
