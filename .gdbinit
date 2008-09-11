define rbt
p __printbt__()
end

define rp
p __show__($arg0)
end

define rps
p __show_simple__($arg0)
end

define validate
p rubinius::VM::current_state()->om->validate_object($arg0)
end
