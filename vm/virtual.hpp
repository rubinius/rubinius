#ifndef RBX_VIRTUAL_HPP
#define RBX_VIRTUAL_HPP

namespace rubinius {

  class ObjectContainer {
  public:
    virtual OBJECT object() = 0;
    virtual ~ObjectContainer() { }
  };

  class ObjectCallback : public ObjectContainer {
  public:
    STATE;

    ObjectCallback(STATE);
    virtual ~ObjectCallback();
    virtual void call(OBJECT) = 0;
  };
}

#endif
