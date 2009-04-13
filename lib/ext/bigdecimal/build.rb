extension do |e|
  e.name 'bigdecimal'
  e.files '*.c'
  e.includes '.'
  e.flags '-DBASE=10000UL -DBASE_FIG=4'
end
