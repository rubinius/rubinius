define rbt
p __printbt__()
end

define rp
p __show__($arg0)
end

define rps
p __show_simple__($arg0)
end

define rph
p __show_subtend__($arg0)
end

define validate
p rubinius::VM::current_state()->om->validate_object($arg0)
end

define refs
p print_references($arg0)
end

define memstat
p x_memstat()
end

define snapshot
p x_snapshot()
end

define print_snapshot
p x_print_snapshot()
end
