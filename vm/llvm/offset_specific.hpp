namespace offset {
namespace Object {
  const static int header = 0;
}
namespace Arguments {
  const static int name = 0;
  const static int recv = 1;
  const static int block = 2;
  const static int total = 3;
  const static int arguments = 4;
  const static int argument_container = 5;
}
namespace Tuple {
  const static int header = 0;
  const static int full_size = 1;
  const static int field = 2;
}
namespace Executable {
  const static int header = 0;
  const static int primitive = 1;
  const static int serial = 2;
  const static int execute = 3;
  const static int prim_index = 4;
  const static int inliners = 5;
}
namespace StackVariables {
  const static int on_heap = 0;
  const static int parent = 1;
  const static int self = 2;
  const static int block = 3;
  const static int module = 4;
  const static int last_match = 5;
  const static int locals = 6;
}
namespace InlineCache {
  const static int name = 0;
  const static int cache = 1;
  const static int call_unit = 2;
  const static int initial = 3;
  const static int execute = 4;
  const static int hits = 5;
  const static int seen_classes_overflow = 6;
  const static int seen_classes = 7;
  const static int private_lock = 8;
}
namespace jit_RuntimeData {
  const static int method = 0;
  const static int name = 1;
  const static int module = 2;
}
namespace Module {
  const static int header = 0;
  const static int method_table = 1;
  const static int module_name = 2;
  const static int constants = 3;
  const static int superclass = 4;
  const static int seen_ivars = 5;
}
namespace Array {
  const static int header = 0;
  const static int total = 1;
  const static int tuple = 2;
  const static int start = 3;
  const static int shared = 4;
}
namespace HeaderWord {
  const static int flags64 = 0;
}
namespace Class {
  const static int header = 0;
  const static int instance_type = 1;
  const static int packed_ivar_info = 2;
  const static int type_info = 3;
  const static int class_id = 4;
  const static int packed_size = 5;
  const static int building = 6;
}
namespace CallFrame {
  const static int previous = 0;
  const static int static_scope = 1;
  const static int dispatch_data = 2;
  const static int cm = 3;
  const static int flags = 4;
  const static int ip = 5;
  const static int jit_data = 6;
  const static int top_scope = 7;
  const static int scope = 8;
  const static int arguments = 9;
  const static int stk = 10;
}
namespace ObjectHeader {
  const static int header = 0;
  const static int klass = 1;
  const static int ivars = 2;
  const static int body = 3;
}
namespace VariableScope {
  const static int header = 0;
  const static int block = 1;
  const static int method = 2;
  const static int module = 3;
  const static int parent = 4;
  const static int heap_locals = 5;
  const static int last_match = 6;
  const static int self = 7;
  const static int number_of_locals = 8;
  const static int isolated = 9;
  const static int locals = 10;
  const static int block_as_method = 11;
}
namespace Numeric {
  const static int header = 0;
}
namespace BlockEnvironment {
  const static int header = 0;
  const static int scope = 1;
  const static int top_scope = 2;
  const static int local_count = 3;
  const static int method = 4;
  const static int module = 5;
}
namespace Dispatch {
  const static int name = 0;
  const static int module = 1;
  const static int method = 2;
  const static int method_missing = 3;
}
namespace Float {
  const static int header = 0;
  const static int val = 1;
}
namespace UnwindInfo {
  const static int target_ip = 0;
  const static int stack_depth = 1;
  const static int type = 2;
}
namespace CompiledMethod {
  const static int header = 0;
  const static int metadata = 1;
  const static int name = 2;
  const static int iseq = 3;
  const static int stack_size = 4;
  const static int local_count = 5;
  const static int required_args = 6;
  const static int post_args = 7;
  const static int total_args = 8;
  const static int splat = 9;
  const static int lines = 10;
  const static int local_names = 11;
  const static int file = 12;
  const static int scope = 13;
  const static int breakpoints = 14;
  const static int backend_method = 15;
  const static int jit_data = 16;
  const static int literals = 17;
}
namespace BlockInvocation {
  const static int flags = 0;
  const static int self = 1;
  const static int static_scope = 2;
  const static int module = 3;
}
namespace ObjectFlags {
  const static int flags = 0;
  const static int aux_word = 1;
}
namespace InlineCacheHit {
  const static int seen_class = 0;
  const static int hits = 1;
}
}
