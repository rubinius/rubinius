#ifndef RBX_TASKPROBE_HPP
#define RBX_TASKPROBE_HPP

#include "builtin/object.hpp"

#include "vm/vmmethod.hpp"
#include "type_info.hpp"

namespace rubinius {

#define PROBE_ALL                       (0xffffffff)
#define PROBE_ADD_METHOD                (1 << 0)
#define PROBE_START_METHOD              (1 << 1)
#define PROBE_LOOKUP_FAILED             (1 << 2)
#define PROBE_EXECUTE_INSTRUCTION       (1 << 3)
#define PROBE_LOAD_RUNTIME              (1 << 4)
#define PROBE_PRIMITIVES                (1 << 5)
#define PROBE_EXECUTE_METHOD            (1 << 6)

#define PROBE_ALL_OP                    "all"
#define PROBE_ALL1_OP                   "1"
#define PROBE_ADD_METHOD_OP             "add_method"
#define PROBE_START_METHOD_OP           "start_method"
#define PROBE_LOOKUP_FAILED_OP          "lookup_failed"
#define PROBE_EXECUTE_INSTRUCTION_OP    "execute_instruction"
#define PROBE_LOAD_RUNTIME_OP           "load_runtime"
#define PROBE_PRIMITIVES_OP             "primitives"
#define PROBE_EXECUTE_METHOD_OP         "execute_method"

  class Task;
  class Module;
  class CompiledMethod;
  class MethodContext;
  class Message;

  /**
   * TaskProbe is a collection of probes that print out data
   * about various VM operations. TaskProbe provides for enabling
   * or disabling the individual probes from Ruby or C++ code.
   */
  class TaskProbe : public Object {
  public:
    const static object_type type = TaskProbeType;

    /**
     * Bit field of probes that are enabled
     */
    size_t flags;

    static void init(STATE);

    // Ruby.primitive :taskprobe_allocate
    static TaskProbe* create(STATE);

    // Ruby.primitive :taskprobe_enable
    Object* enable(STATE, Symbol* probe);

    // Ruby.primitive :taskprobe_disable
    Object* disable(STATE, Symbol* probe);

    // Ruby.primitive :taskprobe_enabled_p
    Object* enabled_p(STATE, Symbol* probe);

    /**
     * Enables a probe specified by its string name. Called
     * by the Ruby primitive enable since Symbols are more
     * natural in Ruby. Also called by the parse_env when
     * setting probes specified in the PROBE env var.
     *
     * @see parse_env
     */
    void enable(std::string probe);

    /**
     * Disables a probe specified by its string name.
     *
     * @see enable
     */
    void disable(std::string probe);

    /**
     * Parses env string for probes to enable. The string should
     * be comma delimited with no spaces. Returns true if any probes
     * were enabled, otherwise returns false. An example string is:
     *
     *   "start_method,execute_instructions"
     *
     * If env is NULL, attempts to parse options from environmental
     * variable PROBE.
     */
    bool parse_env(const char* env);

    /**
     * Given a probe string, returns the corresponding bit field.
     */
    size_t probe_flag(std::string probe);

    /**
     * Returns true if the bit flag is set in flags.
     */
    bool enabled_p(size_t flag);

    /**
     * Prints the name of the Ruby method before the method runs.
     */
    void start_method(Task* task, Message& msg);

    /**
     * Prints the name of the Ruby method added to a method_table.
     */
    void added_method(Task* task, Module* mod, Symbol* name, CompiledMethod *meth);

    /**
     * Prints the module and name of the method being executed.
     *
     *  SomeClass.method for class methods, and
     *  SomeClass#method for instance methods.
     */
    void execute_method(STATE, Task* task, Message& msg);

    /**
     * Prints the name of the Ruby method if unable to locate the
     * method.
     *
     * @todo This is currently invoked from Task::tragic_failure.
     * That method raises an exception, so the lookup failure would
     * be printed anyway. Perhaps the probe would be more useful
     * before missing_method or elsewhere in the lookup algorithm.
     */
    void lookup_failed(Task* task, Message& msg);

    /**
     * Prints the name of the instruction (opcode) before it runs.
     */
    void execute_instruction(Task* task, MethodContext* ctx, opcode op);

    /**
     * Prints the name of the file being loaded from runtime kernel.
     * This probe must be enabled via the PROBE env var.
     */
    void load_runtime(STATE, std::string file);

    /**
     * Prints out the name of the primitive that couldn't be found.
     * This probe must be enable via the PROBE env var.
     */
    void missing_primitive(STATE, std::string prim);

    class Info : public TypeInfo {
    public:
      Info(object_type type): TypeInfo(type) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };
}

#endif
